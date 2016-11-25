#include "ByteBuffer.h"

ProjectC::Networking::ByteBuffer<boost::endian::order::big>::ByteBuffer(uint8_t* buffer, size_t length) : m_bufferStart(buffer), m_current(m_bufferStart), m_bufferEnd(buffer + length)
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

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::Reset(uint8_t* buffer, uint32_t length)
{
	m_bufferStart = buffer;
	m_bufferEnd = buffer + length;
	m_current = m_bufferStart;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteUShort(uint16_t value)
{
	boost::endian::native_to_big_inplace(value);
	WriteByte(static_cast<uint8_t>(value >> 8));
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteUInt(uint32_t value)
{
	boost::endian::native_to_big_inplace(value);
	WriteByte(static_cast<uint8_t>(value >> 24));
	WriteByte(static_cast<uint8_t>(value >> 16));
	WriteByte(static_cast<uint8_t>(value >> 8));
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteULong(uint64_t value)
{
	boost::endian::native_to_big_inplace(value);
	WriteByte(static_cast<uint8_t>(value >> 56));
	WriteByte(static_cast<uint8_t>(value >> 48));
	WriteByte(static_cast<uint8_t>(value >> 40));
	WriteByte(static_cast<uint8_t>(value >> 32));
	WriteByte(static_cast<uint8_t>(value >> 24));
	WriteByte(static_cast<uint8_t>(value >> 16));
	WriteByte(static_cast<uint8_t>(value >> 8));
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteFloat(float_t value)
{
	union {
		uint32_t Integer;
		float_t Float;
	} result;
	result.Float = value;
	WriteUInt(result.Integer);
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteDouble(double_t value)
{
	union {
		uint64_t Integer;
		double_t Double;
	} result;
	result.Double = value;
	WriteULong(result.Integer);
}


float_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetFloat()
{
	union {
		uint32_t Integer;
		float_t Float;
	} result;
	result.Integer = GetUInt();
	return result.Float;
}

double_t ProjectC::Networking::ByteBuffer<boost::endian::order::big>::GetDouble()
{
	union {
		uint64_t Integer;
		double_t Double;
	} result;
	result.Integer = GetULong();
	return result.Double;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteChar(int8_t value)
{
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteShort(int16_t value)
{
	WriteUShort(static_cast<uint16_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteInt(int32_t value)
{
	WriteUInt(static_cast<uint32_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteLong(int64_t value)
{
	WriteULong(static_cast<uint64_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteBytes(const std::vector<uint8_t>& vec, size_t count)
{
	if (Available() < count)
		throw std::overflow_error("Not enough space.");
	memcpy(m_current, vec.data(), count);
	m_current += count;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::big>::WriteBytes(const uint8_t* buffer, size_t count)
{
	if (Available() < count)
		throw std::overflow_error("Not enough space.");
	memcpy(m_current, buffer, count);
	m_current += count;
}

//////////////////////////////////////////////////////////////////////////


ProjectC::Networking::ByteBuffer<boost::endian::order::little>::ByteBuffer(uint8_t* buffer, size_t length) : m_bufferStart(buffer), m_current(m_bufferStart), m_bufferEnd(buffer + length)
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

float_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetFloat()
{
	union {
		uint32_t Integer;
		float_t Float;
	} result;
	result.Integer = GetUInt();
	return result.Float;
}

double_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetDouble()
{
	union {
		uint64_t Integer;
		double_t Double;
	} result;
	result.Integer = GetULong();
	return result.Double;
}

int8_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetChar()
{
	return static_cast<int8_t>(GetByte());
}

int16_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetShort()
{
	return static_cast<int16_t>(GetUShort());
}

int32_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetInt()
{
	return static_cast<int32_t>(GetUInt());
}

int64_t ProjectC::Networking::ByteBuffer<boost::endian::order::little>::GetLong()
{
	return static_cast<int64_t>(GetULong());
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

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::Reset(uint8_t* buffer, uint32_t length)
{
	m_bufferStart = buffer;
	m_bufferEnd = buffer + length;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteUShort(uint16_t value)
{
	boost::endian::native_to_little_inplace(value);
	WriteByte(static_cast<uint8_t>(value >> 8));
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteUInt(uint32_t value)
{
	boost::endian::native_to_little_inplace(value);
	WriteByte(static_cast<uint8_t>(value >> 24));
	WriteByte(static_cast<uint8_t>(value >> 16));
	WriteByte(static_cast<uint8_t>(value >> 8));
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteULong(uint64_t value)
{
	boost::endian::native_to_little_inplace(value);
	WriteByte(static_cast<uint8_t>(value >> 56));
	WriteByte(static_cast<uint8_t>(value >> 48));
	WriteByte(static_cast<uint8_t>(value >> 40));
	WriteByte(static_cast<uint8_t>(value >> 32));
	WriteByte(static_cast<uint8_t>(value >> 24));
	WriteByte(static_cast<uint8_t>(value >> 16));
	WriteByte(static_cast<uint8_t>(value >> 8));
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteFloat(float_t value)
{
	union {
		uint32_t Integer;
		float_t Float;
	} result;
	result.Float = value;
	WriteUInt(result.Integer);
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteDouble(double_t value)
{
	union {
		uint64_t Integer;
		double_t Double;
	} result;
	result.Double = value;
	WriteULong(result.Integer);
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteChar(int8_t value)
{
	WriteByte(static_cast<uint8_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteShort(int16_t value)
{
	WriteUShort(static_cast<uint16_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteInt(int32_t value)
{
	WriteUInt(static_cast<uint32_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteLong(int64_t value)
{
	WriteULong(static_cast<uint64_t>(value));
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteBytes(const std::vector<uint8_t>& vec, size_t count)
{
	if (Available() < count)
		throw std::overflow_error("Not enough space.");
	memcpy(m_current, vec.data(), count);
	m_current += count;
}

void ProjectC::Networking::ByteBuffer<boost::endian::order::little>::WriteBytes(const uint8_t* buffer, size_t count)
{
	if (Available() < count)
		throw std::overflow_error("Not enough space.");
	memcpy(m_current, buffer, count);
	m_current += count;
}