#pragma once
#pragma unmanaged

#include "IServer.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <functional>

namespace ProjectC {
	namespace Networking {
		class ThreadedTcpConnection;

		class TcpServer : public IServer {
		private:
			boost::asio::ip::tcp::acceptor m_acceptor;
			bool m_running{ false };
			ReceiveHandler m_handler{ nullptr };
			ErrorHandler m_errHandler{ nullptr };
		public:
			TcpServer(const boost::asio::ip::tcp::endpoint& endpoint, boost::asio::io_service& io_svc);
			TcpServer(boost::asio::io_service& io_svc);
			~TcpServer();

			bool Bind(const boost::asio::ip::tcp::endpoint& endpoint);
			virtual bool Bind(IPAddress address, uint16_t port) override;
			virtual void Start(ReceiveHandler handler) override;
			virtual void Stop() override;
			virtual void Close() override;

			virtual boost::asio::io_service& GetIOService() override {
				return m_acceptor.get_io_service();
			}
			virtual void SetErrorHandler(ErrorHandler handler) override {
				m_errHandler = handler;
			}
			boost::asio::ip::tcp::acceptor& GetAcceptor() {
				return m_acceptor;
			}
			virtual Endpoint GetEndpoint() const override {
				return Endpoint::Create(m_acceptor.local_endpoint());
			}
			virtual bool IsRunning() const override {
				return m_running;
			}
		private:
			void start_accept();
			void handle_accept(std::shared_ptr<ThreadedTcpConnection> con, const boost::system::error_code& err_code);
		};
	}
}