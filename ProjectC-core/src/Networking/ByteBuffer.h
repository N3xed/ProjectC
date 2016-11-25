#pragma once

#include <boost/endian/conversion.hpp>
#include "IByteBuffer.h"

namespace ProjectC {
	namespace Networking {
		template<boost::endian::order BO>
		class ByteBuffer : public IByteBuffer {};

		template<>
		class ByteBuffer<boost::endian::order::big> : public IByteBuffer {
		private:
			uint8_t* m_bufferStart;
			uint8_t* m_bufferEnd;
			uint8_t* m_current;
		public:
			ByteBuffer(uint8_t* buffer, size_t length);

			virtual boost::endian::order Endian() override;
			virtual size_t Available() override;
			virtual size_t Position() override;
			virtual void SetPosition(size_t pos) override;

			inline virtual uint8_t GetByte() override {
				return *m_current++;
			}
			virtual uint16_t GetUShort() override;
			virtual uint32_t GetUInt() override;
			virtual uint64_t GetULong() override;
			virtual int8_t GetChar() override;
			virtual int16_t GetShort() override;
			virtual int32_t GetInt() override;
			virtual int64_t GetLong() override;
			virtual float_t GetFloat() override;
			virtual double_t GetDouble() override;

			virtual void GetBytes(uint8_t* buffer, size_t count) override;
			virtual void GetBytes(std::vector<uint8_t>& vec, size_t count) override;
			virtual const uint8_t* GetBytes(size_t count) override;
			virtual const uint8_t* GetBytes() override;

			virtual void Advance(size_t count) override;
			virtual void ResetPosition() override;
			virtual void Reset(uint8_t* buffer, uint32_t length) override;

			inline virtual void WriteByte(uint8_t value) override{
				*m_current = value;
				++m_current;
			}
			virtual void WriteUShort(uint16_t value) override;
			virtual void WriteUInt(uint32_t value) override;
			virtual void WriteULong(uint64_t value) override;
			virtual void WriteChar(int8_t value) override;
			virtual void WriteShort(int16_t value) override;
			virtual void WriteInt(int32_t value) override;
			virtual void WriteLong(int64_t value) override;
			virtual void WriteFloat(float_t value) override;
			virtual void WriteDouble(double_t value) override;

			virtual void WriteBytes(const uint8_t* buffer, size_t count) override;
			virtual void WriteBytes(const std::vector<uint8_t>& vec, size_t count) override;
		};

		template<>
		class ByteBuffer<boost::endian::order::little> : public IByteBuffer {
		private:
			uint8_t* m_bufferStart;
			uint8_t* m_bufferEnd;
			uint8_t* m_current;
		public:
			ByteBuffer(uint8_t* buffer, size_t length);

			virtual boost::endian::order Endian() override;
			virtual size_t Available() override;
			virtual size_t Position() override;
			virtual void SetPosition(size_t pos) override;

			inline virtual uint8_t GetByte() override {
				return *m_current++;
			}
			virtual uint16_t GetUShort() override;
			virtual uint32_t GetUInt() override;
			virtual uint64_t GetULong() override;
			virtual int8_t GetChar() override;
			virtual int16_t GetShort() override;
			virtual int32_t GetInt() override;
			virtual int64_t GetLong() override;
			virtual float_t GetFloat() override;
			virtual double_t GetDouble() override;

			virtual void GetBytes(uint8_t* buffer, size_t count) override;
			virtual void GetBytes(std::vector<uint8_t>& vec, size_t count) override;
			virtual const uint8_t* GetBytes(size_t count) override;
			virtual const uint8_t* GetBytes() override;

			virtual void Advance(size_t count) override;
			virtual void ResetPosition() override;
			virtual void Reset(uint8_t* buffer, uint32_t length) override;

			inline virtual void WriteByte(uint8_t value) override {
				*m_current = value;
				++m_current;
			}
			virtual void WriteUShort(uint16_t value) override;
			virtual void WriteUInt(uint32_t value) override;
			virtual void WriteULong(uint64_t value) override;
			virtual void WriteChar(int8_t value) override;
			virtual void WriteShort(int16_t value) override;
			virtual void WriteInt(int32_t value) override;
			virtual void WriteLong(int64_t value) override;
			virtual void WriteFloat(float_t value) override;
			virtual void WriteDouble(double_t value) override;

			virtual void WriteBytes(const uint8_t* buffer, size_t count) override;
			virtual void WriteBytes(const std::vector<uint8_t>& vec, size_t count) override;
		};
	}
}
