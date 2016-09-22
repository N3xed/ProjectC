#pragma once

#include "Endpoint.h"
#include "Container.h"
#include "Packet.h"

namespace ProjectC {
	namespace Networking {
		class IConnection {
		public:
			typedef std::function<void(bool err, const std::exception& ex)> SendHandler;
			typedef std::function<void(IConnection& con)> ReceiveHandler;

			virtual ~IConnection() {};

			virtual Endpoint GetLocalEndpoint() const = 0;
			virtual Endpoint GetRemoteEndpoint() const = 0;
			virtual ProtocolType GetProtocol() const = 0;

			virtual Buffer GetBuffer() const = 0;
			virtual Packet* GetLastPacket() = 0;
			virtual void SetLastPacket(std::weak_ptr<Packet> packet) = 0;

			virtual void Send(const uint8_t* buffer, size_t length) = 0;
			virtual void SendAsync(const uint8_t* buffer, size_t length, SendHandler handler = SendHandler(nullptr)) = 0;
		};
	}
}