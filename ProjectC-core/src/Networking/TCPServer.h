#pragma once
#pragma unmanaged

#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <functional>
#include "TcpConnection.h"

namespace ProjectC {
	namespace Networking {
		class TcpServer {
		private:
			boost::asio::ip::tcp::acceptor m_acceptor;
			bool m_running{ false };
			std::function<void(std::shared_ptr<TcpConnection>)> m_handler{ nullptr };
			std::function<void(const std::exception& err)> m_errHandler{ nullptr };
		public:
			TcpServer(const boost::asio::ip::tcp::endpoint& endpoint, boost::asio::io_service& io_svc);
			TcpServer(boost::asio::io_service& io_svc);
			~TcpServer();

			void SetErrorHandler(std::function<void(const std::exception& err)> handler);

			bool Bind(const boost::asio::ip::tcp::endpoint& endpoint);
			void Start(std::function<void(std::shared_ptr<TcpConnection>)> handler);
			void Stop();
			void Close();

			boost::asio::io_service& GetIOService() {
				return m_acceptor.get_io_service();
			}
			boost::asio::ip::tcp::acceptor& GetAcceptor() {
				return m_acceptor;
			}
			boost::asio::ip::tcp::endpoint GetEndpoint() const {
				return m_acceptor.local_endpoint();
			}
			bool IsRunning() const {
				return m_running;
			}
		private:
			void start_accept();
			void handle_accept(std::shared_ptr<TcpConnection> con, const boost::system::error_code& err_code);
		};
	}
}