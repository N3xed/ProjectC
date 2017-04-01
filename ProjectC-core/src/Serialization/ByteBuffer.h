#pragma once

#include "IByteBuffer.h"

namespace ProjectC {
	namespace Serialization {
		

		class ByteBuffer : public IByteBuffer {
		private:
			uint8_t* m_startPtr;
			uint8_t* m_currentPtr;
			uint8_t* m_endPtr;
			ReadWriteState m_readWriteState;
		public:
			ByteBuffer();
			ByteBuffer(const uint8_t* start, const uint8_t* end);
			ByteBuffer(uint8_t* start, uint8_t* end, ReadWriteState state = ReadWriteState::READ_WRITE);

			virtual inline size_t GetPosition() override { return m_currentPtr - m_startPtr; }
			virtual inline bool CanWrite(size_t amount) override { return m_currentPtr + amount <= m_endPtr; }
			virtual inline void SetPosition(size_t pos) override { m_currentPtr = m_startPtr + pos; }
			virtual inline void SetReadWriteState(ReadWriteState state) override { m_readWriteState = state; }
			virtual inline ReadWriteState GetReadWriteState() const override { return m_readWriteState; }
			virtual inline void Advance(size_t count) override { m_currentPtr += count; if (m_currentPtr > m_endPtr) { throw std::overflow_error("Cannot skip over the end."); m_currentPtr = m_endPtr; } }
			virtual inline void ResetPosition() override { m_currentPtr = m_startPtr; }
			virtual inline size_t Available() const override { return m_currentPtr - m_endPtr; }
			virtual size_t Size() const override { return m_endPtr - m_startPtr; }

			virtual const uint8_t* GetCurrentPtr() const override { return m_currentPtr; }
			virtual const uint8_t* GetStartPtr() const override { return m_startPtr; }
			virtual const uint8_t* GetEndPtr() const override { return m_endPtr; }

			virtual void Reset(uint8_t* buffer, size_t length) override;
			virtual void Reset(const uint8_t* buffer, size_t length) override;

			virtual inline uint8_t Read() override {
				return *m_currentPtr++;
			}
			virtual void ReadAll(uint8_t* dest, size_t count) override;

			virtual inline void Write(uint8_t value) override { 
				*m_currentPtr = value; ++m_currentPtr; 
			}
			virtual void WriteAll(const uint8_t* buffer, size_t size) override;


		};
	}
}
