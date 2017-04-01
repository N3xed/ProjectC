#pragma once
#include <string>

namespace ProjectC::Interface {
	class IPeerInfo {
	public:
		virtual ~IPeerInfo() {}

		virtual inline std::string Name() = 0;
		virtual inline std::string IPAddress() = 0;
		virtual inline std::string Version() = 0;
		virtual inline bool Online() = 0;
		virtual inline uint32_t Id() = 0;

		virtual bool Next() = 0;
		virtual void Reset() = 0;
	};
}