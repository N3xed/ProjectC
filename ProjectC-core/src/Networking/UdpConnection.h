#pragma once
#pragma unmanaged

#include "IConnection.h"
#include "UdpServer.h"

namespace ProjectC {
	namespace Networking {
		class UdpConnection : public IConnection {
		public:
			typedef std::function<void()> CloseHandler;
			
		private:
			friend class UdpServer;

			UdpServer& m_server;
			boost::asio::ip::udp::endpoint m_remoteEndpoint;
			const uint8_t* m_buffer;
			size_t m_bufferSize;
			std::weak_ptr<Packet> m_lastPacket;
			ErrorHandler m_errorHandler{ nullptr };
			CloseHandler m_closeHandler{ nullptr };
		public:
			UdpConnection(UdpServer& server);
			UdpConnection(UdpServer& server, boost::asio::ip::udp::endpoint remoteEndpoint);

			virtual Endpoint GetLocalEndpoint() const override {
				return m_server.GetEndpoint();
			}
			virtual Endpoint GetRemoteEndpoint() const override {
				return Endpoint::Create(m_remoteEndpoint);
			}
			virtual ProtocolType GetProtocolType() const override {
				return ProtocolType::UDP;
			}
			UdpServer& GetServer() {
				return m_server;
			}

			virtual bool Send(const uint8_t* buffer, size_t length) override;
			virtual void SendAsync(const uint8_t* buffer, size_t length, SendHandler handler = SendHandler(nullptr)) override;
			virtual void Close() override;

			virtual std::tuple<const uint8_t*, size_t> GetBuffer() const override;

			virtual Packet* GetLastPacket() override {
				return m_lastPacket.lock().get();
			}

			virtual void SetLastPacket(std::weak_ptr<Packet> packet) override {
				m_lastPacket.swap(packet);
			}
			
			virtual bool IsOpen() const noexcept override {
				return true;
			}

			virtual void SetErrorHandler(ErrorHandler handler) override {
				m_errorHandler = handler;
			}
			void SetCloseHandler(CloseHandler handler) {
				m_closeHandler = handler;
			}
			virtual void SetDisconnectedHandler(DisconnectedHandler handler) override {
				assert(false);
				throw std::logic_error("Operation not supported.");
			}
		};
	}
}