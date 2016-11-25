#include "ConnectionManager.h"
#include "UdpConnection.h"
#include "ThreadedTcpConnection.h"

namespace ProjectC::Networking {

	ConnectionManager::ConnectionManager() : m_ioService(new boost::asio::io_service()), m_udpServer(*m_ioService), m_tcpServer(*m_ioService), m_eventError(), m_eventLostConnection(), m_eventReceivedData(), m_eventReceivedConnection()
	{ 
		m_udpServer.SetErrorHandler([this](const std::exception& ex) {
			EventError()(*this, RoutedException::Create<UdpServer>(&m_udpServer, ex));
		});
		m_tcpServer.SetErrorHandler([this](const std::exception& ex) {
			EventError()(*this, RoutedException::Create<TcpServer>(&m_tcpServer, ex));
		});
	}

	ConnectionManager::~ConnectionManager()
	{
		delete m_ioService;
	}

	void ConnectionManager::StartThread()
	{
		if (m_running)
			return;
		m_running = true;
		m_serverThread = ThreadManager::Create<ThreadType::NETWORKING>([this]() {
			try {
				m_ioService->run();
			}
			catch (boost::system::error_code& err) {
				EventError()(*this, RoutedException::Create(this, std::exception(err.message().c_str(), err.value())));
			}
			m_running = false;
		});
	}

	void ConnectionManager::StopThread()
	{
		assert(m_serverThread != nullptr);
		if (!m_running)
			return;
		m_running = false;
		m_ioService->stop();
		m_serverThread->join();
		m_ioService->reset();
	}

	void ConnectionManager::StopThreadAsync(Callback callback)
	{
		ThreadManager::Create<ThreadType::NETWORKING>([this, callback]() {
			StopThread();
			callback(true);
		});
	}

	bool ProjectC::Networking::ConnectionManager::TryStartTcp(const IPAddress& addr, uint16_t port)
	{
		if (!m_tcpServer.Bind(boost::asio::ip::tcp::endpoint(addr, port)))
			return false;
		m_tcpServer.Start([this](std::shared_ptr<IConnection> con) {
			con->SetErrorHandler([this](IConnection& con, std::exception& ex) {
				EventError()(*this, RoutedException::Create(&con, ex));
			});
			static_cast<TcpConnection*>(con.get())->SetReceiveHandler([this](IConnection& con) {
				EventReceivedData()(*this, con);
			});
			con->SetDisconnectedHandler([this, con](IConnection& conref) {
				EventLostConnection()(*this, conref);
				RemoveConnection(con);
			});
			AddConnection(con);
			EventReceivedConnection()(*this, *con);
			static_cast<TcpConnection*>(con.get())->Start();
		});
		return true;
	}

	bool ProjectC::Networking::ConnectionManager::TryStartUdp(const IPAddress& addr, uint16_t port)
	{
		if (!m_udpServer.Bind(boost::asio::ip::udp::endpoint(addr, port)))
			return false;
		m_udpServer.Start([this](std::shared_ptr<IConnection> con) {
			con->SetErrorHandler([this](IConnection& con, std::exception& ex) {
				EventError()(*this, RoutedException::Create(&con, ex));
			});
			static_cast<UdpConnection*>(con.get())->SetCloseHandler([]() {});
			EventReceivedData()(*this, *con);
			con.reset();
		});
		return true;
	}

	void ProjectC::Networking::ConnectionManager::StopUdp()
	{
		m_udpServer.Stop();
	}

	void ProjectC::Networking::ConnectionManager::StopTcp()
	{
		m_tcpServer.Stop();
	}
	
	IConnection* ProjectC::Networking::ConnectionManager::ConnectTcp(const IPAddress& addr, uint16_t port)
	{
		auto con = std::make_shared<ThreadedTcpConnection>();
		con->SetErrorHandler([this](IConnection& con, std::exception& ex) {
			EventError()(*this, RoutedException::Create(&con, ex));
		});
		con->SetReceiveHandler([this](IConnection& con) {
			EventReceivedData()(*this, con);
		});
		con->SetDisconnectedHandler([this, con](IConnection& conref) {
			EventLostConnection()(*this, conref);
			RemoveConnection(con);
		});
		if (!con->Connect(boost::asio::ip::tcp::endpoint(addr, port)))
			return nullptr;
		AddConnection(con);
		con->Start();
		return static_cast<IConnection*>(con.get());
	}

	IConnection* ProjectC::Networking::ConnectionManager::ConnectUdp(const IPAddress& addr, uint16_t port)
	{
		auto con = std::make_shared<UdpConnection>(m_udpServer, boost::asio::ip::udp::endpoint(addr, port));
		con->SetErrorHandler([this](IConnection& con, std::exception& ex) {
			EventError()(*this, RoutedException::Create(&con, ex));
		});
		con->SetCloseHandler([this, con]() {
			RemoveConnection(con);
		});
		AddConnection(con);
		return static_cast<IConnection*>(con.get());
	}

	void ProjectC::Networking::ConnectionManager::CloseTcp()
	{
		m_tcpServer.Close();
	}

	void ProjectC::Networking::ConnectionManager::CloseUdp()
	{
		m_udpServer.Close();
	}
}