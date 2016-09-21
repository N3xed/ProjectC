#pragma once
#pragma unmanaged

#include <stdint.h>
#include <array>
#include <vector>
#include <boost/endian/conversion.hpp>

namespace ProjectC {
	namespace Networking {
		class IByteBuffer {
		public:
			virtual ~IByteBuffer() {};

			virtual boost::endian::order Endian() = 0;
			virtual size_t Available() = 0;
			virtual size_t Position() = 0;
			virtual void SetPosition(size_t pos) = 0;
			virtual void Advance(size_t count) = 0;
			virtual void ResetPosition() = 0;
			virtual void Reset(const uint8_t* buffer, uint32_t length) = 0;

			inline virtual uint8_t GetByte() = 0;
			virtual uint16_t GetUShort() = 0;
			virtual uint32_t GetUInt() = 0;
			virtual uint64_t GetULong() = 0;
			virtual int8_t GetChar() = 0;
			virtual int16_t GetShort() = 0;
			virtual int32_t GetInt() = 0;
			virtual int64_t GetLong() = 0;

			virtual void GetBytes(uint8_t* buffer, size_t count) = 0;
			virtual void GetBytes(std::vector<uint8_t>& vec, size_t count) = 0;
			template<size_t count>
			std::array<uint8_t, count> GetBytes() {
				std::array<uint8_t, count> result;
				GetBytes(result.data(), count);
				return std::move(result);
			}
			virtual const uint8_t* GetBytes(size_t count) = 0;
			virtual const uint8_t* GetBytes() = 0;
		};
	}
}