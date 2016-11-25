#pragma once

#include <vector>
#include <map>
#include "UdpServer.h"
#include "TcpServer.h"
#include "../InvocationList.h"
#include "../RoutedException.h"
#include <mutex>
#include "../ThreadManager.h"

namespace ProjectC::Networking {
	
	class ConnectionManager {
	public:
		typedef std::function<void(bool success)> Callback;
		typedef std::function<void(IConnection*, bool success)> ConnectCallback;
	private:
		InvocationList<void(ConnectionManager&, RoutedException&)> m_eventError;
		InvocationList<void(ConnectionManager&, IConnection&)> m_eventReceivedConnection;
		InvocationList<void(ConnectionManager&, IConnection&)> m_eventLostConnection;
		InvocationList<void(ConnectionManager&, IConnection&)> m_eventReceivedData;
		
		std::mutex m_connectionMutex;
		std::vector<std::shared_ptr<IConnection>> m_connections;
		boost::thread* m_serverThread;
		bool m_running{ false };
		
		boost::asio::io_service* m_ioService;
		UdpServer m_udpServer;
		TcpServer m_tcpServer;
	public:
		InvocationList<void(ConnectionManager&, RoutedException&)>& EventError() {
			return m_eventError;
		}
		InvocationList<void(ConnectionManager&, IConnection&)>& EventReceivedConnection() {
			return m_eventReceivedConnection;
		}
		InvocationList<void(ConnectionManager&, IConnection&)>& EventLostConnection() {
			return m_eventLostConnection;
		}
		InvocationList<void(ConnectionManager&, IConnection&)>& EventReceivedData() {
			return m_eventReceivedData;
		}

		ConnectionManager();
		~ConnectionManager();

		template<ProtocolType Protocol>
		bool TryStart(const IPAddress& addr, uint16_t port);
		template<> bool TryStart<ProtocolType::TCP>(const IPAddress& addr, uint16_t port) {
			return TryStartTcp(addr, port);
		}
		template<> bool TryStart<ProtocolType::UDP>(const IPAddress& addr, uint16_t port) {
			return TryStartUdp(addr, port);
		}

		template<ProtocolType protocol>
		void StartAsync(IPAddress addr, uint16_t port, Callback callback) {
			assert(protocol != ProtocolType::NONE);
			ThreadManager::Create<ThreadType::NETWORKING>([this, addr, port, callback]() {
				bool success = TryStart<protocol>(addr, port);
				callback(success);
			});
		}

		template<ProtocolType protocol> 
		void Stop();
		template<> void Stop<ProtocolType::TCP>() {
			StopTcp();
		}
		template<> void Stop<ProtocolType::UDP>() {
			StopUdp();
		}

		template<ProtocolType protocol>
		void StopAsync(Callback callback) {
			assert(protocol != ProtocolType::NONE);
			ThreadManager::Create<ThreadType::NETWORKING>([this, callback]() {
				Stop<protocol>();
				callback(true);
			});
		}

		template<ProtocolType protocol>
		IConnection* Connect(const IPAddress& addr, uint16_t port);
		template<> IConnection* Connect<ProtocolType::TCP>(const IPAddress& addr, uint16_t port) {
			return ConnectTcp(addr, port);
		}
		template<> IConnection* Connect<ProtocolType::UDP>(const IPAddress& addr, uint16_t port) {
			return ConnectUdp(addr, port);
		}

		template<ProtocolType protocol>
		void ConnectAsync(IPAddress addr, uint16_t port, ConnectCallback callback) {
			assert(protocol != ProtocolType::NONE);
			ThreadManager::Create<ThreadType::NETWORKING>([this, addr, port, callback]() {
				auto ptr = Connect<protocol>(addr, port);
				callback(ptr, ptr != nullptr);
			});
		}

		template<ProtocolType protocol>
		void Close();
		template<> void Close<ProtocolType::TCP>() {
			CloseTcp();
		}
		template<> void Close<ProtocolType::UDP>() {
			CloseUdp();
		}

		template<ProtocolType protocol>
		void CloseAsync(Callback callback) {
			assert(protocol != ProtocolType::NONE);
			ThreadManager::Create<ThreadType::NETWORKING>([this, callback]() {
				Close<protocol>();
				callback(true);
			});
		}

		template<ProtocolType protocol>
		bool IsOpen();
		template<> bool IsOpen<ProtocolType::TCP>() {
			return m_tcpServer.GetAcceptor().is_open();
		}
		template<> bool IsOpen<ProtocolType::UDP>() {
			return m_udpServer.GetSocket().is_open();
		}

		std::vector<std::shared_ptr<IConnection>>& GetConnections() {
			return m_connections;
		}
		boost::asio::io_service& GetIOService() {
			return *m_ioService;
		}
		bool IsRunning() const {
			return m_running;
		}

		void StartThread();
		void StopThread();


		void StopThreadAsync(Callback callback);

	protected:

		bool TryStartUdp(const IPAddress& addr, uint16_t port);
		bool TryStartTcp(const IPAddress& addr, uint16_t port);

		void StopUdp();
		void StopTcp();

		IConnection* ConnectTcp(const IPAddress& addr, uint16_t port);
		IConnection* ConnectUdp(const IPAddress& addr, uint16_t port);

		void CloseUdp();
		void CloseTcp();

	private:

		inline void AddConnection(std::shared_ptr<IConnection> con) {
			std::lock_guard<std::mutex> lock{ m_connectionMutex };
			m_connections.emplace_back(std::move(con));
		}
		inline void RemoveConnection(std::shared_ptr<IConnection> con) {
			std::lock_guard<std::mutex> lock{ m_connectionMutex };
			auto it = std::find_if(m_connections.begin(), m_connections.end(), [&con](auto e) {
				return e.get() == con.get();
			});
			assert(it != m_connections.end());
			m_connections.erase(it);
		}
	};

}