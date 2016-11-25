#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

namespace ProjectC {
	namespace Networking {
		typedef boost::asio::ip::address IPAddress;

		enum class ProtocolType: uint8_t {
			NONE = 0,
			TCP = 1,
			UDP = 2
		};

		class Endpoint {
		public:
			boost::asio::ip::address Address;
			uint16_t Port;
			ProtocolType Protocol;

			Endpoint();
			Endpoint(const boost::asio::ip::address& address, uint16_t port, ProtocolType protocol);

			static Endpoint Create(const boost::asio::ip::tcp::endpoint& endpoint) {
				return Endpoint(endpoint.address(), endpoint.port(), ProtocolType::TCP);
			}
			static Endpoint Create(const boost::asio::ip::udp::endpoint& endpoint) {
				return Endpoint(endpoint.address(), endpoint.port(), ProtocolType::UDP);
			}

			operator boost::asio::ip::tcp::endpoint() {
				return boost::asio::ip::tcp::endpoint(Address, Port);
			}
			operator boost::asio::ip::udp::endpoint() {
				return boost::asio::ip::udp::endpoint(Address, Port);
			}
		};
	}
}