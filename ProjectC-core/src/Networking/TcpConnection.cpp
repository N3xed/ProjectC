#include "TcpConnection.h"

ProjectC::Networking::TcpConnection::TcpConnection(boost::asio::io_service& io_service) : m_socket(io_service)
{}

ProjectC::Networking::TcpConnection::~TcpConnection()
{
	Close();
}

void ProjectC::Networking::TcpConnection::Start()
{
	assert(!m_closed);
	assert(m_receiveHandler);

	m_running = true;
	m_buffer = std::make_unique<uint8_t[]>(MAX_PACKET_SIZE);
	start_receive();
}

void ProjectC::Networking::TcpConnection::Stop()
{
	m_running = false;
	m_buffer.reset();
	m_bufferDataSize = 0;
}

bool ProjectC::Networking::TcpConnection::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	try {
		m_socket.connect(endpoint);
		m_endpoint = endpoint;
	}
	catch (const boost::system::error_code& err_code) {
		m_errorHandler(*this, std::exception(err_code.message().c_str(), err_code.value()));
		return false;
	}
	return true;
}

void ProjectC::Networking::TcpConnection::AsyncConnect(const boost::asio::ip::tcp::endpoint& endpoint, ConnectHandler handler)
{
	m_socket.async_connect(endpoint, boost::bind(&TcpConnection::connect_handler, this, boost::asio::placeholders::error, handler));
}

void ProjectC::Networking::TcpConnection::Close()
{
	if (m_closed)
		return;
	m_closed = true;
	if (m_running) {
		Stop();
	}
	m_disconnectHandler(*this);
	try {
		m_socket.close();
	}
	catch (const boost::system::error_code& err_code) {
		m_errorHandler(*this, std::exception(err_code.message().c_str(), err_code.value()));
	}
}

bool ProjectC::Networking::TcpConnection::Send(const uint8_t* buffer, size_t length)
{
	assert(!m_closed);
	assert(length > 0);
	assert(buffer != nullptr);

	try {
		boost::asio::write(m_socket, boost::asio::buffer(buffer, length));
	}
	catch (boost::system::error_code& ex) {
		switch (ex.value()) {
		case boost::asio::error::not_connected:
		case boost::asio::error::connection_reset:
		case boost::asio::error::connection_aborted:
		case boost::asio::error::eof:
			Close();
		default:
			m_errorHandler(*this, std::exception(ex.message().c_str(), ex.value()));
		}
		return false;
	}
	return true;
}

void ProjectC::Networking::TcpConnection::SendAsync(const uint8_t* buffer, size_t length, SendHandler handler)
{
	assert(!m_closed);
	assert(buffer != nullptr);
	assert(length > 0);

	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, length), [this, handler](const boost::system::error_code& errCode, size_t bytesWritten) {
		switch (errCode.value()) {
		case boost::system::errc::success:
			break;
		case boost::asio::error::not_connected:
		case boost::asio::error::connection_reset:
		case boost::asio::error::connection_aborted:
		case boost::asio::error::eof:
			Close();
		}

		handler(errCode.value() != boost::system::errc::success, std::exception(errCode.message().c_str(), errCode.value()));
	});
}

std::tuple<const uint8_t*, size_t> ProjectC::Networking::TcpConnection::GetBuffer() const
{
	return std::make_tuple(const_cast<const uint8_t*>(m_buffer.get()), m_bufferDataSize);
}

ProjectC::Networking::Packet* ProjectC::Networking::TcpConnection::GetLastPacket()
{
	return m_lastPacket.lock().get();
}

void ProjectC::Networking::TcpConnection::SetLastPacket(std::weak_ptr<Packet> packet)
{
	m_lastPacket.swap(packet);
}

void ProjectC::Networking::TcpConnection::connect_handler(const boost::system::error_code& err_code, ConnectHandler handler)
{
	if (err_code.value() == boost::system::errc::success) {
		m_endpoint = m_socket.remote_endpoint();
	}
	handler(err_code.value() != boost::system::errc::success, std::exception(err_code.message().c_str(), err_code.value()));
}

void ProjectC::Networking::TcpConnection::start_receive()
{
	m_socket.async_receive(boost::asio::buffer(m_buffer.get(), MAX_PACKET_SIZE), boost::bind(&TcpConnection::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ProjectC::Networking::TcpConnection::handle_receive(const boost::system::error_code& err_code, size_t length)
{
	if (!m_running)
		return;
	if (err_code) {
		switch (err_code.value()) {
		case boost::asio::error::not_connected:
		case boost::asio::error::connection_aborted:
		case boost::asio::error::connection_reset:
		case boost::asio::error::eof:
			Close();
		default:
			m_errorHandler(*this, std::exception(err_code.message().c_str(), err_code.value()));
			break;
		}
	}
	else {
		m_bufferDataSize = length;
		m_receiveHandler(*this);
		memset(m_buffer.get(), 0, length);
		start_receive();
	}
}
