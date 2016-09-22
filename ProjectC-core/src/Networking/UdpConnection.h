#pragma once
#pragma unmanaged

#include "IConnection.h"
#include "UdpServer.h"

namespace ProjectC {
	namespace Networking {
		class UdpConnection : public IConnection {
		private:
			friend class UdpServer;

			UdpServer& m_server;
			boost::asio::ip::udp::endpoint m_remoteEndpoint;
			Buffer m_buffer;
			std::weak_ptr<Packet> m_lastPacket;
		public:
			UdpConnection(UdpServer& server) : m_server(server), m_buffer(0, server.m_buffer.get())
			{}

			virtual Endpoint GetLocalEndpoint() const override
			{
				return Endpoint::FromBoost(m_server.GetEndpoint());
			}
			virtual Endpoint GetRemoteEndpoint() const override
			{
				return Endpoint::FromBoost(m_remoteEndpoint);
			}
			virtual ProtocolType GetProtocol() const override
			{
				return ProtocolType::UDP;
			}

			UdpServer& GetServer() {
				return m_server;
			}

			virtual void Send(const uint8_t* buffer, size_t length) override
			{
				m_server.GetSocket().send_to(boost::asio::buffer(buffer, length), m_remoteEndpoint);
			}

			virtual void SendAsync(const uint8_t* buffer, size_t length, SendHandler handler = SendHandler(nullptr)) override
			{
				m_server.GetSocket().async_send_to(boost::asio::buffer(buffer, length), m_remoteEndpoint, [&handler](const boost::system::error_code& errCode, size_t bytesTransferred) -> void{
					handler(errCode.value() == 0, std::exception(errCode.message().c_str(), errCode.value()));
				});
			}

			virtual Buffer GetBuffer() const override
			{
				return m_buffer;
			}

			virtual Packet* GetLastPacket() override
			{
				return m_lastPacket.lock().get();
			}

			virtual void SetLastPacket(std::weak_ptr<Packet> packet) override
			{
				m_lastPacket.swap(packet);
			}
		};
	}
}