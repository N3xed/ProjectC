#pragma once

#include "TcpConnection.h"
#include <boost/thread.hpp>

namespace ProjectC::Networking {
	class ThreadedTcpConnection : public TcpConnection {
	private:
		boost::asio::io_service m_ioService;
	public:
		ThreadedTcpConnection() : TcpConnection(m_ioService)
		{}

		virtual void Start() override {
			m_ioService.reset();
			TcpConnection::Start();
			boost::thread thread{ boost::bind(&boost::asio::io_service::run, &m_ioService) };
		}

		virtual void Stop() override {
			TcpConnection::Stop();
			if (!m_ioService.stopped())
				m_ioService.stop();
		}
	};
}
