#pragma once
#include "IByteReader.h"
#include "IByteWriter.h"

namespace ProjectC::Serialization {

	template<typename T>
	class IBinaryConverter {
	public:
		virtual ~IBinaryConverter() {}
		virtual void Deserialize(IByteReader& reader, T& result) = 0;
		virtual void Serialize(IByteWriter& writer, const T& obj) = 0;
		virtual size_t Size(const T& obj) const = 0;
	};

}