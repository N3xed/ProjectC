#include "UdpServer.h"
#include "UdpConnection.h"

ProjectC::Networking::UdpServer::UdpServer(boost::asio::io_service& svc) : m_socket(svc)
{}

ProjectC::Networking::UdpServer::UdpServer(const boost::asio::ip::udp::endpoint& endpoint, boost::asio::io_service& svc) : m_socket(svc, endpoint)
{}

bool ProjectC::Networking::UdpServer::Bind(const boost::asio::ip::udp::endpoint& endpoint)
{
	try {
		m_socket.open(endpoint.protocol());
		m_socket.bind(endpoint);
	}
	catch (const boost::system::error_code& errCode) {
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
		return false;
	}
	return true;
}

bool ProjectC::Networking::UdpServer::Bind(IPAddress address, uint16_t port)
{
	return Bind(boost::asio::ip::udp::endpoint(address, port));
}

void ProjectC::Networking::UdpServer::Start(ReceiveHandler handler)
{
	m_running = true;
	m_handler = handler;
	m_buffer = std::make_unique<uint8_t[]>(MAX_PACKET_SIZE);
	start_receive();
}

void ProjectC::Networking::UdpServer::Stop()
{
	try {
		m_running = false;
		auto endpoint = m_socket.local_endpoint();
		Close();
		Bind(endpoint);
		m_buffer.reset();
	} 
	catch (const boost::system::error_code& errCode) {
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
	}
}

void ProjectC::Networking::UdpServer::Close()
{
	if (!m_socket.is_open())
		return;
	if (m_running)
		Stop();
	try {
		m_socket.shutdown(boost::asio::socket_base::shutdown_both);
		m_socket.close();
	}
	catch (const boost::system::error_code& errCode) {
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
	}
}

void ProjectC::Networking::UdpServer::start_receive()
{
	auto con = std::make_shared<UdpConnection>(*this);
	m_socket.async_receive_from(boost::asio::buffer(m_buffer.get(), MAX_PACKET_SIZE), con->m_remoteEndpoint, boost::bind(&UdpServer::handler_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, con));
}

void ProjectC::Networking::UdpServer::handler_receive(const boost::system::error_code& errCode, size_t length, std::shared_ptr<UdpConnection> con)
{
	if (!m_running)
		return;
	if (errCode) {
		m_running = false;
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
	}
	else {
		con->m_bufferSize = length;
		m_handler(std::move(con));
		memset(m_buffer.get(), 0, length);
		start_receive();
	}
}

