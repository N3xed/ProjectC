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

			ReceiveHandler m_receiveHandler{ nullptr };
			ErrorHandler m_errorHandler{ nullptr };
			DisconnectedHandler m_disconnectHandler{ nullptr };
			bool m_running{ false };
			bool m_closed{ false };
			std::unique_ptr<uint8_t[]> m_buffer;
			size_t m_bufferDataSize;
			std::weak_ptr<Packet> m_lastPacket;
		public:
			TcpConnection(boost::asio::io_service& io_service);
			~TcpConnection();

			boost::asio::ip::tcp::socket& GetSocket() {
				return m_socket;
			}

			bool IsRunning() const {
				return m_running;
			}

			virtual void Start();
			virtual void Stop();

			bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);
			void AsyncConnect(const boost::asio::ip::tcp::endpoint& endpoint, ConnectHandler handler);
			virtual void Close() override;

			virtual bool Send(const uint8_t* buffer, size_t length) override;
			virtual void SendAsync(const uint8_t* buffer, size_t length, SendHandler handler = SendHandler(nullptr)) override;

			virtual Endpoint GetLocalEndpoint() const override {
				return Endpoint::Create(m_socket.local_endpoint());
			}
			virtual Endpoint GetRemoteEndpoint() const override {
				return Endpoint::Create(m_socket.remote_endpoint());
			}
			virtual ProtocolType GetProtocolType() const override {
				return ProtocolType::TCP;
			}

			virtual std::tuple<const uint8_t*, size_t> GetBuffer() const override;
			virtual Packet* GetLastPacket() override;
			virtual void SetLastPacket(std::weak_ptr<Packet> packet) override;

			virtual bool IsOpen() const noexcept override {
				return m_closed;
			}

			void SetReceiveHandler(ReceiveHandler handler) {
				m_receiveHandler = handler;
			}
			virtual void SetErrorHandler(ErrorHandler handler) override {
				m_errorHandler = handler;
			}
			virtual void SetDisconnectedHandler(DisconnectedHandler handler) override {
				m_disconnectHandler = handler;
			}
			
		private:
			void connect_handler(const boost::system::error_code& err_code, ConnectHandler handler);
			void start_receive();
			void handle_receive(const boost::system::error_code& err_code, size_t length);
		};
	}
}