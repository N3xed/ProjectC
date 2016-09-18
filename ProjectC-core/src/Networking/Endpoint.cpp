#include "Endpoint.h"

ProjectC::Networking::Endpoint::Endpoint(const boost::asio::ip::address& address, uint16_t port, ProtocolType protocol) : Address(address), Port(port), Protocol(protocol)
{}

ProjectC::Networking::Endpoint::Endpoint()
{}
