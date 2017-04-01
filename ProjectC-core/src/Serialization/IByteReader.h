#pragma once

#include <cassert>
#include <boost/endian/conversion.hpp>
#include <cstdint>
#include "IReader.h"

namespace ProjectC::Serialization {

	typedef boost::endian::order ByteOrder;

	class IByteReader : public IReader<uint8_t> {
	public:
		virtual ~IByteReader() {}

	public:
		template<typename T, ByteOrder Endian = ByteOrder::native> T Read();

		template<> uint8_t Read<uint8_t, ByteOrder::native>() {
			return IReader<uint8_t>::Read();
		}

		template<ByteOrder Endian = ByteOrder::native> uint16_t Read() {
			if (Available() < sizeof(uint16_t))
				throw std::overflow_error("Not enough bytes.");
			uint16_t result = static_cast<uint16_t>(
				static_cast<uint16_t>(IReader<uint8_t>::Read()) << 8 |
				static_cast<uint16_t>(IReader<uint8_t>::Read())
				);
			boost::endian::conditional_reverse_inplace<Endian, ByteOrder::native>(result);
			return result;
		}

		template<ByteOrder Endian = ByteOrder::native> uint32_t Read() {
			if (Available() < sizeof(uint32_t))
				throw std::overflow_error("Not enough bytes.");
			uint32_t result = static_cast<uint32_t>(
				static_cast<uint32_t>(IReader<uint8_t>::Read()) << 24 |
				static_cast<uint32_t>(IReader<uint8_t>::Read()) << 16 |
				static_cast<uint32_t>(IReader<uint8_t>::Read()) << 8 |
				static_cast<uint32_t>(IReader<uint8_t>::Read())
				);
			boost::endian::conditional_reverse_inplace<Endian, ByteOrder::native>(result);
			return result;
		}

		template<ByteOrder Endian = ByteOrder::native> uint64_t Read() {
			if (Available() < sizeof(uint64_t))
				throw std::overflow_error("Not enough bytes.");
			uint64_t result = static_cast<uint64_t>(
				static_cast<uint64_t>(IReader<uint8_t>::Read()) << 56 |
				static_cast<uint64_t>(IReader<uint8_t>::Read()) << 48 |
				static_cast<uint64_t>(IReader<uint8_t>::Read()) << 40 |
				static_cast<uint64_t>(IReader<uint8_t>::Read()) << 32 |
				static_cast<uint64_t>(IReader<uint8_t>::Read()) << 24 |
				static_cast<uint64_t>(IReader<uint8_t>::Read()) << 16 |
				static_cast<uint64_t>(IReader<uint8_t>::Read()) << 8 |
				static_cast<uint64_t>(IReader<uint8_t>::Read())
				);
			boost::endian::conditional_reverse_inplace<Endian, ByteOrder::native>(result);
			return result;
		}

		template<ByteOrder Endian = ByteOrder::native> float Read() {
			union {
				uint32_t a;
				float b;
			} result{ Read<uint32_t, Endian>() };
			return result.b;
		}
		
		template<ByteOrder Endian = ByteOrder::native> double Read() {
			union {
				uint64_t a;
				double b;
			} result{ Read<uint64_t, Endian>() };
			return result.b;
		}

		template<> int8_t Read<int8_t, ByteOrder::native>() {
			return static_cast<int8_t>(Read<uint8_t>());
		}

		template<ByteOrder Endian = ByteOrder::native> int16_t Read() {
			return static_cast<int16_t>(Read<uint16_t, Endian>());
		}

		template<ByteOrder Endian = ByteOrder::native> int32_t Read() {
			return static_cast<int32_t>(Read<uint32_t, Endian>());
		}

		template<ByteOrder Endian = ByteOrder::native> int64_t Read() {
			return static_cast<int64_t>(Read<uint64_t, Endian>());
		}

		template<typename T> void Read(T* buffer, size_t size) {
			if (Available() < size)
				throw std::overflow_error("Not enough bytes.");
			for (size_t i = 0; i < size; ++i) {
				buffer[i] = Read<T>();
			}
		}
	};
}