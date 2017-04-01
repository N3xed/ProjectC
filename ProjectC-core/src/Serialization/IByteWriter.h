#pragma once
#include <cstdint>
#include <boost/endian/conversion.hpp>
#include "IWriter.h"

namespace ProjectC::Serialization {

	typedef boost::endian::order ByteOrder;

	class IByteWriter : public IWriter<uint8_t> {
	public:
		virtual ~IByteWriter() {}

		template<typename T, ByteOrder Endian = ByteOrder::native> void Write(T value);

		template<> void Write<uint8_t, ByteOrder::native>(uint8_t value) {
			IWriter<uint8_t>::Write(value);
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(uint16_t value) {
			if (!CanWrite(sizeof(uint16_t)))
				throw std::overflow_error("Cannot write.");

			boost::endian::conditional_reverse_inplace<ByteOrder::native, Endian>(value);
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 8));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value));
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(uint32_t value) {
			if (!CanWrite(sizeof(uint32_t)))
				throw std::overflow_error("Cannot write.");

			boost::endian::conditional_reverse_inplace<ByteOrder::native, Endian>(value);
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 24));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 16));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 8));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value));
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(uint64_t value) {
			if (!CanWrite(sizeof(uint64_t)))
				throw std::overflow_error("Cannot write.");

			boost::endian::conditional_reverse_inplace<ByteOrder::native, Endian>(value);
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 56));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 48));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 40));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 32));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 24));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 16));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value >> 8));
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value));
		}

		template<> void Write<int8_t, ByteOrder::native>(int8_t value) {
			IWriter<uint8_t>::Write(static_cast<uint8_t>(value));
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(int16_t value) {
			Write<uint16_t, Endian>(static_cast<uint16_t>(value));
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(int32_t value) {
			Write<uint32_t, Endian>(static_cast<uint32_t>(value));
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(int64_t value) {
			Write<uint64_t, Endian>(static_cast<uint64_t>(value));
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(float value) {
			union {
				uint32_t a;
				float b;
			} result{ value };
			Write<uint32_t, Endian>(result.a);
		}

		template<ByteOrder Endian = ByteOrder::native> void Write(double value) {
			union {
				uint64_t a;
				double b;
			} result{ value };
			Write<uint64_t, Endian>(result.a);
		}
	};
}
