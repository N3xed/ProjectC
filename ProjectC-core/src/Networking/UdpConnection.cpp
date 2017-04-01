#include "UdpConnection.h"



ProjectC::Networking::UdpConnection::UdpConnection(UdpServer& server, boost::asio::ip::udp::endpoint remoteEndpoint) : m_server(server), m_buffer(server.m_buffer.get()), m_remoteEndpoint(remoteEndpoint)
{ }

ProjectC::Networking::UdpConnection::UdpConnection(UdpServer& server) : m_server(server), m_buffer(server.m_buffer.get())
{ }

bool ProjectC::Networking::UdpConnection::Send(const uint8_t* buffer, size_t length)
{
	try {
		m_server.GetSocket().send_to(boost::asio::buffer(buffer, length), m_remoteEndpoint);
	}
	catch (boost::system::error_code& errCode) {
		m_errorHandler(*this, std::exception(errCode.message().c_str(), errCode.value()));
		return false;
	}
	return true;
}

void ProjectC::Networking::UdpConnection::SendAsync(const uint8_t* buffer, size_t length, SendHandler handler /*= SendHandler(nullptr)*/)
{
	m_server.GetSocket().async_send_to(boost::asio::buffer(buffer, length), m_remoteEndpoint, [&handler](const boost::system::error_code& errCode, size_t bytesTransferred) -> void {
		handler(errCode.value() == 0, std::exception(errCode.message().c_str(), errCode.value()));
	});
}

std::tuple<const uint8_t*, size_t> ProjectC::Networking::UdpConnection::GetBuffer() const
{
	return std::make_tuple(const_cast<const uint8_t*>(m_buffer), m_bufferSize);
}

void ProjectC::Networking::UdpConnection::Close()
{
	assert(m_closeHandler);
	m_closeHandler();
}