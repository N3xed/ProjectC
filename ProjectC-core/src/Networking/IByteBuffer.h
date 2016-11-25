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
			virtual void Reset(uint8_t* buffer, uint32_t length) = 0;

			inline virtual uint8_t GetByte() = 0;
			virtual uint16_t GetUShort() = 0;
			virtual uint32_t GetUInt() = 0;
			virtual uint64_t GetULong() = 0;
			virtual int8_t GetChar() = 0;
			virtual int16_t GetShort() = 0;
			virtual int32_t GetInt() = 0;
			virtual int64_t GetLong() = 0;
			virtual float_t GetFloat() = 0;
			virtual double_t GetDouble() = 0;

			inline virtual void WriteByte(uint8_t value) = 0;
			virtual void WriteUShort(uint16_t value) = 0;
			virtual void WriteUInt(uint32_t value) = 0;
			virtual void WriteULong(uint64_t value) = 0;
			virtual void WriteChar(int8_t value) = 0;
			virtual void WriteShort(int16_t value) = 0;
			virtual void WriteInt(int32_t value) = 0;
			virtual void WriteLong(int64_t value) = 0;
			virtual void WriteFloat(float_t value) = 0;
			virtual void WriteDouble(double_t value) = 0;

			virtual void WriteBytes(const uint8_t* buffer, size_t count) = 0;
			virtual void WriteBytes(const std::vector<uint8_t>& vec, size_t count) = 0;
			template<size_t count>
			void WriteBytes(const std::array<uint8_t, count>& buffer) {
				WriteBytes(buffer.data(), buffer.size());
			}

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