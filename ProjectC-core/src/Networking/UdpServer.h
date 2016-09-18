#pragma once
#pragma unmanaged
#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace ProjectC {
	namespace Networking {
		class UdpServer {
		public:
			static const uint32_t MAX_PACKET_SIZE = 8192;

			typedef std::function<void(const uint8_t* data, size_t length, const boost::asio::ip::udp::endpoint& endpoint)> ReadHandler;
			typedef std::function<void(const std::exception& exception)> ErrorHandler;
		private:
			boost::asio::ip::udp::socket m_socket;

			bool m_running{ false };
			ReadHandler m_handler{ nullptr };
			ErrorHandler m_errHandler{ nullptr };

			std::unique_ptr<uint8_t[]> m_buffer{ nullptr };
		public:
			UdpServer(boost::asio::io_service& svc);
			UdpServer(const boost::asio::ip::udp::endpoint& endpoint, boost::asio::io_service& svc);

			void SetErrorHandler(ErrorHandler handler) {
				m_errHandler = handler;
			}

			bool Bind(const boost::asio::ip::udp::endpoint& endpoint);
			void Start(ReadHandler handler);
			void Stop();
			void Close();

			boost::asio::io_service& GetIOService() {
				return m_socket.get_io_service();
			}
			boost::asio::ip::udp::socket& GetSocket() {
				return m_socket;
			}
			boost::asio::ip::udp::endpoint GetEndpoint() const {
				return m_socket.local_endpoint();
			}
			bool IsRunning() const {
				return m_running;
			}
		private:
			void start_receive();
			void handler_receive(const boost::system::error_code& errCode, size_t length, std::shared_ptr<boost::asio::ip::udp::endpoint> endpoint);
		};
	}
}