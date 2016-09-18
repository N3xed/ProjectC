#include "UdpServer.h"

ProjectC::Networking::UdpServer::UdpServer(boost::asio::io_service& svc) : m_socket(svc)
{}

ProjectC::Networking::UdpServer::UdpServer(const boost::asio::ip::udp::endpoint& endpoint, boost::asio::io_service& svc) : m_socket(svc, endpoint)
{}

bool ProjectC::Networking::UdpServer::Bind(const boost::asio::ip::udp::endpoint& endpoint)
{
	try {
		m_socket.bind(endpoint);
	}
	catch (const boost::system::error_code& errCode) {
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
		return false;
	}
	return true;
}

void ProjectC::Networking::UdpServer::Start(ReadHandler handler)
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
		m_buffer.reset();
	} 
	catch (const boost::system::error_code& errCode) {
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
	}
}

void ProjectC::Networking::UdpServer::Close()
{
	if (m_running)
		Stop();
	try {
		m_socket.close();
	}
	catch (const boost::system::error_code& errCode) {
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
	}
}

void ProjectC::Networking::UdpServer::start_receive()
{
	std::shared_ptr<boost::asio::ip::udp::endpoint> endpoint = std::make_shared<boost::asio::ip::udp::endpoint>();
	m_socket.async_receive_from(boost::asio::buffer(m_buffer.get(), MAX_PACKET_SIZE), *endpoint, boost::bind(&UdpServer::handler_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, endpoint));
}

void ProjectC::Networking::UdpServer::handler_receive(const boost::system::error_code& errCode, size_t length, std::shared_ptr<boost::asio::ip::udp::endpoint> endpoint)
{
	if (!m_running)
		return;
	if (errCode) {
		m_errHandler(std::exception(errCode.message().c_str(), errCode.value()));
	}
	else {
		m_handler(m_buffer.get(), length, *endpoint);
	}
	start_receive();
}

