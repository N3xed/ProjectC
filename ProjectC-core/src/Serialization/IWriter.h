#pragma once


namespace ProjectC::Serialization {
	
	template<typename T>
	class IWriter {
	public:
		virtual ~IWriter() {}

		virtual inline void Write(T value) = 0;
		virtual inline void WriteAll(const T* buffer, size_t size) = 0;

		virtual bool CanWrite(size_t amount) = 0;
	};
}