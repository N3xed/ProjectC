#include "TcpServer.h"

ProjectC::Networking::TcpServer::TcpServer(const boost::asio::ip::tcp::endpoint& endpoint, boost::asio::io_service& io_svc) : m_acceptor(io_svc, endpoint)
{}

ProjectC::Networking::TcpServer::~TcpServer()
{}

void ProjectC::Networking::TcpServer::SetErrorHandler(std::function<void(const std::exception& err)> handler)
{
	m_errHandler = handler;
}

bool ProjectC::Networking::TcpServer::Bind(const boost::asio::ip::tcp::endpoint& endpoint)
{
	try {
		m_acceptor.open(endpoint.protocol());
		m_acceptor.bind(endpoint);
	}
	catch (const boost::system::error_code& err) {
		m_errHandler(std::exception(err.message().c_str(), err.value()));
		return false;
	}
	return true;
}

ProjectC::Networking::TcpServer::TcpServer(boost::asio::io_service& io_svc) : m_running(false), m_acceptor(io_svc)
{}

void ProjectC::Networking::TcpServer::Start(std::function<void(std::shared_ptr<TcpConnection>)> handler)
{
	m_handler = handler;
	m_running = true;
	start_accept();
}

void ProjectC::Networking::TcpServer::Stop()
{
	try {
		m_running = false;
		m_acceptor.cancel();
	}
	catch (const boost::system::error_code& err_code) {
		m_errHandler(std::exception(err_code.message().c_str(), err_code.value()));
	}
}

void ProjectC::Networking::TcpServer::Close()
{
	try {
		m_running = false;
		m_acceptor.close();
	}
	catch (const boost::system::error_code& err_code) {
		m_errHandler(std::exception(err_code.message().c_str(), err_code.value()));
	}
}

void ProjectC::Networking::TcpServer::start_accept()
{
	std::shared_ptr<TcpConnection> con = std::make_shared<TcpConnection>(m_acceptor.get_io_service());
	m_acceptor.async_accept(con->m_socket, con->m_endpoint, boost::bind(&TcpServer::handle_accept, this, std::move(con), boost::asio::placeholders::error));
}

void ProjectC::Networking::TcpServer::handle_accept(std::shared_ptr<TcpConnection> con, const boost::system::error_code& err_code)
{
	if (err_code) {
		m_errHandler(std::exception(err_code.message().c_str(), err_code.value()));
	} else {
		m_handler(con);
		if (m_running)
			start_accept();
	}
}

