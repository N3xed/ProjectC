#pragma once
#pragma unmanaged

#include <boost/asio.hpp>

namespace ProjectC {
	namespace Networking {

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

			static Endpoint FromBoost(const boost::asio::ip::tcp::endpoint& endpoint) {
				return Endpoint(endpoint.address(), endpoint.port(), ProtocolType::TCP);
			}
			static Endpoint FromBoost(const boost::asio::ip::udp::endpoint& endpoint) {
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