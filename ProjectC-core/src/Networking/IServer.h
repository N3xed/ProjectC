#pragma once
#include "Endpoint.h"

class boost::asio::io_service;

namespace ProjectC::Networking {
	class IConnection;

	class IServer {
	public:
		typedef std::function<void(std::exception& ex)> ErrorHandler;
		typedef std::function<void(std::shared_ptr<IConnection> ptr)> ReceiveHandler;

		virtual bool Bind(IPAddress address, uint16_t port) = 0;
		virtual void Close() = 0;

		virtual void Start(ReceiveHandler handler) = 0;
		virtual void Stop() = 0;

		virtual boost::asio::io_service& GetIOService() = 0;
		virtual Endpoint GetEndpoint() const = 0;
		virtual bool IsRunning() const = 0;
		virtual void SetErrorHandler(ErrorHandler handler) = 0;
	};
}