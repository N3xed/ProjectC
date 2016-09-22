#include "TcpConnection.h"

ProjectC::Networking::TcpConnection::TcpConnection(boost::asio::io_service& io_service) : m_socket(io_service)
{}

ProjectC::Networking::TcpConnection::~TcpConnection()
{}

void ProjectC::Networking::TcpConnection::Start()
{
	m_running = true;
	m_buffer.DataPtr = std::make_unique<uint8_t[]>(MAX_PACKET_SIZE);
	start_receive();
}

void ProjectC::Networking::TcpConnection::Stop()
{
	try {
		m_running = false;
		m_buffer.DataPtr.reset();
	}
	catch (const boost::system::error_code& err_code) {
		m_errorHandler(std::exception(err_code.message().c_str(), err_code.value()));
	}
}

bool ProjectC::Networking::TcpConnection::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	try {
		m_socket.connect(endpoint);
		m_endpoint = endpoint;
	}
	catch (const boost::system::error_code& err_code) {
		m_errorHandler(std::exception(err_code.message().c_str(), err_code.value()));
		return false;
	}
	return true;
}

void ProjectC::Networking::TcpConnection::AsyncConnect(const boost::asio::ip::tcp::endpoint& endpoint, std::function<void(bool, std::exception)> handler)
{
	m_socket.async_connect(endpoint, boost::bind(&TcpConnection::connect_handler, this, boost::asio::placeholders::error, handler));
}

void ProjectC::Networking::TcpConnection::Close()
{
	if (m_running)
		Stop();
	try {
		m_socket.close();
	}
	catch (const boost::system::error_code& err_code) {
		m_errorHandler(std::exception(err_code.message().c_str(), err_code.value()));
	}
}

void ProjectC::Networking::TcpConnection::Send(const uint8_t* buffer, size_t length)
{
	boost::asio::write(m_socket, boost::asio::buffer(buffer, length));
}

void ProjectC::Networking::TcpConnection::SendAsync(const uint8_t* buffer, size_t length, SendHandler handler)
{
	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, length), [&handler](const boost::system::error_code& errCode, size_t bytesWritten) -> void{
		handler(errCode.value() == 0, std::exception(errCode.message().c_str(), errCode.value()));
	});
}

ProjectC::Networking::Buffer ProjectC::Networking::TcpConnection::GetBuffer() const
{
	return Buffer(m_buffer.Length, m_buffer.DataPtr.get());
}

ProjectC::Networking::Packet* ProjectC::Networking::TcpConnection::GetLastPacket()
{
	return m_lastPacket.lock().get();
}

void ProjectC::Networking::TcpConnection::SetLastPacket(std::weak_ptr<Packet> packet)
{
	m_lastPacket.swap(packet);
}

void ProjectC::Networking::TcpConnection::connect_handler(const boost::system::error_code& err_code, std::function<void(bool, std::exception)> handler)
{
	if (!err_code) {
		m_endpoint = m_socket.remote_endpoint();
	}
	handler(err_code.value() == 0, std::exception(err_code.message().c_str(), err_code.value()));
}

void ProjectC::Networking::TcpConnection::start_receive()
{
	m_socket.async_receive(boost::asio::buffer(m_buffer.DataPtr.get(), MAX_PACKET_SIZE), boost::bind(&TcpConnection::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ProjectC::Networking::TcpConnection::handle_receive(const boost::system::error_code& err_code, size_t length)
{
	if (!m_running)
		return;
	if (err_code) {
		m_errorHandler(std::exception(err_code.message().c_str(), err_code.value()));
	}
	else {
		m_buffer.Length = length;
		m_receiveHandler(*this);
		memset(m_buffer.DataPtr.get(), 0, length);
	}
	start_receive();
}
