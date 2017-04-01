#pragma once
#pragma unmanaged

#include <stdint.h>
#include <array>
#include <vector>
#include "IByteReader.h"
#include "IByteWriter.h"

namespace ProjectC {
	namespace Serialization {

		enum class ReadWriteState : uint32_t {
			NONE = 0,
			READ = 1,
			WRITE = 2,
			READ_WRITE = 3
		};

		class IByteBuffer : public IByteReader, public IByteWriter {
		public:
			virtual ~IByteBuffer() {};

			virtual size_t GetPosition() = 0;
			virtual void SetPosition(size_t pos) = 0;
			virtual void Advance(size_t count) = 0;
			virtual void ResetPosition() = 0;
			virtual void Reset(uint8_t* buffer, size_t length) = 0;
			virtual void Reset(const uint8_t* buffer, size_t length) = 0;
			virtual void SetReadWriteState(ReadWriteState state) = 0;
			virtual ReadWriteState GetReadWriteState() const = 0;
			virtual size_t Size() const = 0;

			virtual const uint8_t* GetCurrentPtr() const = 0;
			virtual const uint8_t* GetStartPtr() const = 0;
			virtual const uint8_t* GetEndPtr() const = 0;

			template<size_t count>
			void WriteBytes(const std::array<uint8_t, count>& buffer) {
				WriteAll(buffer.data(), count);
			}

			template<typename Iterator, boost::endian::order Endian = boost::endian::order::native>
			void WriteBytes(Iterator begin, Iterator end) {
				for (Iterator i = begin; begin != end; ++i) {
					Write<Endian>(*i);
				}
			}

			template<size_t count>
			std::array<uint8_t, count> GetBytes() {
				std::array<uint8_t, count> result;
				ReadAll(result.data(), count);
				return std::move(result);
			}
		};
	}
}