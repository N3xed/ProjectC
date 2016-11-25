#pragma once

#include "Endpoint.h"
#include "Container.h"
#include "Packet.h"

namespace ProjectC {
	namespace Networking {

		class ExecutionContext;

		class IConnection {
		public:
			typedef std::function<void(bool err, std::exception& ex)> SendHandler;
			typedef std::function<void(bool err, std::exception& ex)> ConnectHandler;
			typedef std::function<void(IConnection& con)> ReceiveHandler;
			typedef std::function<void(IConnection& con, std::exception& ex)> ErrorHandler;
			typedef std::function<void(IConnection& con)> DisconnectedHandler;

			virtual ~IConnection() {};

			virtual Endpoint GetLocalEndpoint() const = 0;
			virtual Endpoint GetRemoteEndpoint() const = 0;
			virtual ProtocolType GetProtocolType() const = 0;

			virtual void Close() = 0;
			virtual bool IsOpen() const noexcept = 0;

			virtual void SetErrorHandler(ErrorHandler handler) = 0;
			virtual void SetDisconnectedHandler(DisconnectedHandler handler) = 0;

			virtual Buffer GetBuffer() const = 0;
			virtual Packet* GetLastPacket() = 0;
			virtual void SetLastPacket(std::weak_ptr<Packet> packet) = 0;

			virtual bool Send(const uint8_t* buffer, size_t length) = 0;
			virtual void SendAsync(const uint8_t* buffer, size_t length, SendHandler handler = SendHandler(nullptr)) = 0;
		};
	}
}