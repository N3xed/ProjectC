#pragma once
#include "IBinaryConverter.h"
#include <string>

namespace ProjectC::Serialization {
	template<typename T>
	class BinaryConverter : public IBinaryConverter<T> {
	public:
		virtual void Deserialize(IByteReader& reader, T& result) override { result = reader.Read<T>(); }
		virtual void Serialize(IByteWriter& writer, const T& obj) override { writer.Write<T>(obj); }
		virtual size_t Size(const T& obj) const override { return sizeof(T); }
	};

	template<>
	class BinaryConverter<std::string> : public IBinaryConverter<std::string> {
		virtual void Deserialize(IByteReader& reader, std::string& result) override
		{
			uint16_t size = reader.Read<uint16_t>();
			char* buf = new char(size);
			reader.ReadAll(reinterpret_cast<uint8_t*>(buf), size);
			result.assign(buf, size);
			delete buf;
		}
		virtual void Serialize(IByteWriter& writer, const std::string& obj) override
		{
			assert(obj.size() <= sizeof(uint16_t));
			writer.Write<uint16_t>(obj.size());
			writer.WriteAll(reinterpret_cast<const uint8_t*>(obj.data()), obj.size());
		}
		virtual size_t Size(const std::string& obj) const override
		{
			return obj.size() + 4;
		}
	};
}