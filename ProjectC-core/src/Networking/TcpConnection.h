#pragma once
#pragma unmanaged

#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "IConnection.h"

namespace ProjectC {
	namespace Networking {
		class TcpConnection : public IConnection {
			static const uint32_t MAX_PACKET_SIZE = 8192;

			friend class TcpServer;
			boost::asio::ip::tcp::socket m_socket;
			boost::asio::ip::tcp::endpoint m_endpoint;

			std::function<void(const uint8_t* data, uint32_t length)> m_receiveHandler{ nullptr };
			std::function<void(std::exception& err)> m_errorHandler{ nullptr };
			bool m_running{ false };
			std::unique_ptr<uint8_t[]> m_buffer{ nullptr };
		public:
			TcpConnection(boost::asio::io_service& io_service);
			~TcpConnection();

			void SetReceiveHandler(std::function<void(const uint8_t* data, uint32_t length)> handler) {
				m_receiveHandler = handler;
			}
			boost::asio::ip::tcp::socket& GetSocket() {
				return m_socket;
			}

			bool IsRunning() {
				return m_running;
			}

			void Start();
			void Stop();

			bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);
			void AsyncConnect(const boost::asio::ip::tcp::endpoint& endpoint, std::function<void(bool, std::exception)> handler);
			void Close();

			virtual void Send(const uint8_t* buffer, size_t length) override;
			virtual void SendAsync(const uint8_t* buffer, size_t length, SendHandler handler = SendHandler(nullptr)) override;

			virtual Endpoint GetLocalEndpoint() const override {
				return Endpoint::FromBoost(m_socket.local_endpoint());
			}
			virtual Endpoint GetRemoteEndpoint() const override {
				return Endpoint::FromBoost(m_socket.remote_endpoint());
			}
			virtual ProtocolType GetProtocol() const override {
				return ProtocolType::TCP;
			}

		private:
			void connect_handler(const boost::system::error_code& err_code, std::function<void(bool, std::exception)> handler);
			void start_receive();
			void handle_receive(const boost::system::error_code& err_code, size_t length);
		};
	}
}