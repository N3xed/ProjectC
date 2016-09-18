#pragma once

#include "Endpoint.h"

namespace ProjectC {
	namespace Networking {
		class Connection {
		public:
			typedef std::function<void(bool, const std::exception&)> SendHandler;

			virtual ~Connection() {};

			virtual Endpoint GetLocalEndpoint() const = 0;
			virtual Endpoint GetRemoteEndpoint() const = 0;
			virtual ProtocolType GetProtocol() const = 0;

			virtual void Send(const uint8_t* buffer, size_t length) = 0;
			virtual void SendAsync(const uint8_t* buffer, size_t length, SendHandler handler = SendHandler(nullptr)) = 0;
		};
	}
}