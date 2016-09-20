#pragma once
#pragma unmanaged

#include "IConnection.h"
#include "UdpServer.h"

namespace ProjectC {
	namespace Networking {
		class UdpConnection : public IConnection {
		private:
			UdpServer& m_server;
			Endpoint m_localEndpoint;
			Endpoint m_remoteEndpoint;
		public:
			UdpConnection(UdpServer& server, boost::asio::ip::udp::endpoint remoteEndpoint) : m_server(server), m_localEndpoint(Endpoint::FromBoost(server.GetEndpoint())),
				m_remoteEndpoint(Endpoint::FromBoost(remoteEndpoint))
			{}

			virtual Endpoint GetLocalEndpoint() const override
			{
				return m_localEndpoint;
			}
			virtual Endpoint GetRemoteEndpoint() const override
			{
				return m_remoteEndpoint;
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

		};
	}
}