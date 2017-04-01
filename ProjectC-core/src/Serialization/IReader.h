#pragma once

namespace ProjectC::Serialization {
	template<typename T>
	class IReader {
	public:
		virtual ~IReader() {}

		virtual T Read() = 0;
		virtual void ReadAll(T* dest, size_t count) = 0;
		virtual size_t Available() const = 0;
	};
}