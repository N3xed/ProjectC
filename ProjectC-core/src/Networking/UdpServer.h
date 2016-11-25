#pragma once
#include "IServer.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace ProjectC {
	namespace Networking {
		class UdpConnection;
		class UdpServer : public IServer {
		public:
			friend class UdpConnection;

			static const uint32_t MAX_PACKET_SIZE = 8192;
		private:
			boost::asio::ip::udp::socket m_socket;

			bool m_running{ false };
			ReceiveHandler m_handler{ nullptr };
			ErrorHandler m_errHandler{ nullptr };

			std::unique_ptr<uint8_t[]> m_buffer{ nullptr };
		public:
			UdpServer(boost::asio::io_service& svc);
			UdpServer(const boost::asio::ip::udp::endpoint& endpoint, boost::asio::io_service& svc);

			void SetErrorHandler(ErrorHandler handler) {
				m_errHandler = handler;
			}

			bool Bind(const boost::asio::ip::udp::endpoint& endpoint);
			virtual bool Bind(IPAddress address, uint16_t port) override;
			virtual void Start(ReceiveHandler handler) override;
			virtual void Stop() override;
			virtual void Close() override;

			virtual boost::asio::io_service& GetIOService() override {
				return m_socket.get_io_service();
			}
			boost::asio::ip::udp::socket& GetSocket() {
				return m_socket;
			}
			virtual Endpoint GetEndpoint() const override {
				return Endpoint::Create(m_socket.local_endpoint());
			}
			virtual bool IsRunning() const override {
				return m_running;
			}
		private:
			void start_receive();
			void handler_receive(const boost::system::error_code& errCode, size_t length, std::shared_ptr<UdpConnection> con);
		};
	}
}