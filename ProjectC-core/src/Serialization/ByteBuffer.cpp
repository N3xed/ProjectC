#include "ByteBuffer.h"

ProjectC::Serialization::ByteBuffer::ByteBuffer(const uint8_t* start, const uint8_t* end) : m_startPtr(const_cast<uint8_t*>(start)), m_endPtr(const_cast<uint8_t*>(end)), m_currentPtr(m_startPtr), m_readWriteState(ReadWriteState::READ)
{ }

ProjectC::Serialization::ByteBuffer::ByteBuffer() : m_startPtr(nullptr), m_endPtr(nullptr), m_currentPtr(nullptr), m_readWriteState(ReadWriteState::NONE)
{ }

ProjectC::Serialization::ByteBuffer::ByteBuffer(uint8_t* start, uint8_t* end, ReadWriteState state) : m_startPtr(start), m_currentPtr(start), m_endPtr(end), m_readWriteState(state)
{ }

void ProjectC::Serialization::ByteBuffer::ReadAll(uint8_t* dest, size_t count)
{
	if ((static_cast<uint32_t>(m_readWriteState) & static_cast<uint32_t>(ReadWriteState::READ)) != static_cast<uint32_t>(ReadWriteState::READ))
		throw std::logic_error("Invalid ByteBuffer state.");
	if (Available() < count)
		throw std::overflow_error("Not enough bytes.");
	std::memcpy(dest, m_currentPtr, count);
	m_currentPtr += count;
}

void ProjectC::Serialization::ByteBuffer::WriteAll(const uint8_t* buffer, size_t size)
{
	if ((static_cast<uint32_t>(m_readWriteState) & static_cast<uint32_t>(ReadWriteState::WRITE)) != static_cast<uint32_t>(ReadWriteState::WRITE))
		throw std::logic_error("Invalid ByteBuffer state.");
	if (!CanWrite(size))
		throw std::overflow_error("Cannot write.");
	std::memcpy(m_currentPtr, buffer, size);
	m_currentPtr += size;
}

void ProjectC::Serialization::ByteBuffer::Reset(uint8_t* buffer, size_t length)
{
	m_startPtr = buffer;
	m_currentPtr = buffer;
	m_endPtr = buffer + length;
}

void ProjectC::Serialization::ByteBuffer::Reset(const uint8_t* buffer, size_t length)
{
	m_readWriteState = ReadWriteState::READ;
	Reset(const_cast<uint8_t*>(buffer), length);
}
