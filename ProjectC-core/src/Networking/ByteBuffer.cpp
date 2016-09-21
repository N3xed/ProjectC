#include "ByteBuffer.h"

ProjectC::Networking::ByteBuffer<boost::endian::order::big>::ByteBuffer(const uint8_t* buffer, size_t length) : m_bufferStart(buffer), m_current(m_bufferStart), m_bufferEnd(buffer + length)
{}

boost::endian::order ProjectC::Networking::ByteBuffer<boost::endian::order::big>::Endian()
{
	return boost::endian::order::big;
}

size_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::Available()
{
	return m_bufferEnd - m_current;
}

size_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::Position()
{
	return m_current - m_bufferStart;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::SetPosition(size_t pos)
{
	m_current = m_bufferStart + pos;
}

inline uint8_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetByte()
{
	return *m_current++;
}

uint16_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetUShort()
{
	uint16_t result = (uint16_t)(
		(uint16_t)GetByte() << 8 |
		(uint16_t)GetByte()
		);
	boost::endian::big_to_native_inplace(result);
	return result;
}

uint32_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetUInt()
{
	uint32_t result = (uint32_t)(
		(uint32_t)GetByte() << 24 |
		(uint32_t)GetByte() << 16 |
		(uint32_t)GetByte() << 8 |
		(uint32_t)GetByte()
		);
	boost::endian::big_to_native_inplace(result);
	return result;
}

uint64_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetULong()
{
	uint64_t result = (uint64_t)(
		(uint64_t)GetByte() << 56 |
		(uint64_t)GetByte() << 48 |
		(uint64_t)GetByte() << 40 |
		(uint64_t)GetByte() << 32 |
		(uint64_t)GetByte() << 24 |
		(uint64_t)GetByte() << 16 |
		(uint64_t)GetByte() << 8 |
		(uint64_t)GetByte()
		);
	boost::endian::big_to_native_inplace(result);
	return result;
}

int8_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetChar()
{
	return (int8_t)GetByte();
}

int16_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetShort()
{
	return (int16_t)GetUShort();
}

int32_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetInt()
{
	return (int32_t)GetUInt();
}

int64_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetLong()
{
	return (int64_t)GetULong();
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetBytes(uint8_t* buffer, size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes available.");
	memcpy(buffer, m_current, count);
	m_current += count;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetBytes(std::vector<uint8_t>& vec, size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes available.");
	vec.insert(vec.end(), m_current, m_current + count);
	m_current += count;
}

const uint8_t* ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetBytes(size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes available.");
	auto result = m_current;
	m_current += count;
	return result;
}

const uint8_t* ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetBytes()
{
	auto result = m_current;
	m_current = m_bufferEnd;
	return result;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::Advance(size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes to advance.");
	m_current += count;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::ResetPosition()
{
	m_current = m_bufferStart;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::Reset(const uint8_t* buffer, uint32_t length)
{
	m_bufferStart = buffer;
	m_bufferEnd = buffer + length;
	m_current = m_bufferStart;
}


//////////////////////////////////////////////////////////////////////////


ProjectC::Networking::ByteBuffer<boost::endian::order::little>::ByteBuffer(const uint8_t* buffer, size_t length) : m_bufferStart(buffer), m_current(m_bufferStart), m_bufferEnd(buffer + length)
{}

boost::endian::order ProjectC::Networking::ByteBuffer<boost::endian::order::little>::Endian()
{
	return boost::endian::order::little;
}

size_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::Available()
{
	return m_bufferEnd - m_current;
}

size_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::Position()
{
	return m_current - m_bufferStart;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::SetPosition(size_t pos)
{
	m_current = m_bufferStart + pos;
}

inline uint8_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetByte()
{
	return *m_current++;
}

uint16_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetUShort()
{
	uint16_t result = (uint16_t)(
		(uint16_t)GetByte() << 8 |
		(uint16_t)GetByte()
		);
	boost::endian::little_to_native_inplace(result);
	return result;
}

uint32_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetUInt()
{
	uint32_t result = (uint32_t)(
		(uint32_t)GetByte() << 24 |
		(uint32_t)GetByte() << 16 |
		(uint32_t)GetByte() << 8 |
		(uint32_t)GetByte()
		);
	boost::endian::little_to_native_inplace(result);
	return result;
}

uint64_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetULong()
{
	uint64_t result = (uint64_t)(
		(uint64_t)GetByte() << 56 |
		(uint64_t)GetByte() << 48 |
		(uint64_t)GetByte() << 40 |
		(uint64_t)GetByte() << 32 |
		(uint64_t)GetByte() << 24 |
		(uint64_t)GetByte() << 16 |
		(uint64_t)GetByte() << 8 |
		(uint64_t)GetByte()
		);
	boost::endian::little_to_native_inplace(result);
	return result;
}

int8_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetChar()
{
	return (int8_t)GetByte();
}

int16_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetShort()
{
	return (int16_t)GetUShort();
}

int32_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetInt()
{
	return (int32_t)GetUInt();
}

int64_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetLong()
{
	return (int64_t)GetULong();
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetBytes(uint8_t* buffer, size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes available.");
	memcpy(buffer, m_current, count);
	m_current += count;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetBytes(std::vector<uint8_t>& vec, size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes available.");
	vec.insert(vec.end(), m_current, m_current + count);
	m_current += count;
}

const uint8_t* ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetBytes(size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes available.");
	auto result = m_current;
	m_current += count;
	return result;
}

const uint8_t* ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetBytes()
{
	auto result = m_current;
	m_current = m_bufferEnd;
	return result;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::Advance(size_t count)
{
	if (Available() < count)
		throw std::underflow_error("Not enough bytes to advance.");
	m_current += count;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::ResetPosition()
{
	m_current = m_bufferStart;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::Reset(const uint8_t* buffer, uint32_t length)
{
	m_bufferStart = buffer;
	m_bufferEnd = buffer + length;
}