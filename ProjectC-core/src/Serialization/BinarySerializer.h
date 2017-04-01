#pragma once

#include <unordered_map>
#include "BinaryConverter.h"
#include <vector>

namespace ProjectC::Serialization {

	class IBinaryConvertable {
	public:
		virtual ~IBinaryConvertable() {}
		virtual void Deserialize(IByteReader& reader) = 0;
		virtual void Serialize(IByteWriter& writer) = 0;
		virtual size_t Size() const = 0;
	};

	template<boost::endian::order Endian = boost::endian::order::native, typename Allocator = std::allocator<uint8_t>>
	class BinarySerializer {
	public:
		typedef Container<uint8_t> ContainerType;
	private:
		std::unordered_map<std::string, ContainerType> m_objects;

		Allocator m_allocator;
		bool m_isSerializer;

		class ByteReaderImpl : public IByteReader {
		public:
			const uint8_t* positionPtr;
			const uint8_t* endPtr;

			virtual inline uint8_t Read() override { return *positionPtr++; }
			virtual void ReadAll(uint8_t* dest, size_t count) override {
				if (positionPtr + count > endPtr) throw std::overflow_error("No more bytes.");
				std::memcpy(dest, positionPtr, count);
			}
			virtual inline size_t Available() const override { return endPtr - positionPtr; }

			ByteReaderImpl(const uint8_t* buffer, const uint8_t* end) : positionPtr(buffer), endPtr(end) {}
		};
		class ByteWriterImpl : public IByteWriter {
		public:
			uint8_t* positionPtr;
			uint8_t* endPtr;

			virtual inline void Write(uint8_t value) override { assert(positionPtr + 1 != endPtr); *positionPtr++ = value; }
			virtual inline void WriteAll(const uint8_t* buffer, size_t size) override {
				assert(positionPtr + size < endPtr);
				std::memcpy(positionPtr, buffer, size);
				positionPtr += size;
			}
			virtual bool CanWrite(size_t amount) override { return positionPtr + amount <= endPtr; }
			
			ByteWriterImpl(uint8_t* buffer, uint8_t* end) : positionPtr(buffer), endPtr(end) {}
		};

	public:
		BinarySerializer() : m_isSerializer(true) {
		}
		~BinarySerializer() {
			for (auto& e : m_objects) {
				m_allocator.deallocate(e.second.DataPtr, e.second.Length);
			}
		}

		template<typename T>
		void Put(std::string key, const T& value, IBinaryConverter<T>& converter = BinaryConverter<T>()) {
			assert(m_isSerializer);

			size_t size = converter.Size(value);
			uint8_t* buf = m_allocator.allocate(size);
			
			converter.Serialize(ByteWriterImpl(buf, buf + size), value);

			m_objects.emplace(key, ContainerType(size, buf));
		}
		void Put(std::string key, const IBinaryConvertable& obj) {
			assert(m_isSerializer);
			size_t size = obj.Size();
			uint8_t* buf = m_allocator.allocate(size);

			obj.Serialize(ByteWriterImpl(buf, buf + size));
			m_objects.emplace(key, ContainerType(size, buf));
		}

		template<typename T, typename Iter>
		void Put(std::string key, Iter iter, size_t count, IBinaryConverter<T>& converter = BinaryConverter<T>()) {
			assert(m_isSerializer);
			Iter endIter = iter + count;
			size_t objSize = converter.Size(*iter);
			size_t size = 2 + count * objSize;
			uint8_t* buf = m_allocator.allocate(size);
			ByteWriterImpl writer(buf, buf + size);
			writer.Write<uint16_t, Endian>(count);
			for (Iter i = iter; i != endIter; ++i) {
				assert(converter.Size(*i) == objSize);
				converter.Serialize(writer, *i);
			}

			m_objects.emplace(key, ContainerType(size, buf));
		}


		template<typename T>
		T Get(const std::string& key, IBinaryConverter<T>& converter = BinaryConverter<T>()) {
			assert(!m_isSerializer);

			auto& e = m_objects.at(key);
			T result;
			converter.Deserialize(ByteReaderImpl(e.DataPtr, e.DataPtr + e.Length), result);
			return result;
		}
		template<typename T, typename ResultAllocator = std::allocator<T>>
		T* GetPtr(const std::string& key, IBinaryConverter<T>& converter = BinaryConverter<T>(), ResultAllocator& alloc = std::allocator<T>()) {
			assert(!m_isSerializer);
			T* result = alloc.allocate(1);
			converter.Deserialize(ByteReaderImpl(e.DataPtr, e.DataPtr + e.Length), *result);
			return result;
		}

		template<typename T, typename Container>
		void Get(const std::string& key, Container& container, IBinaryConverter<T>& converter = BinaryConverter<T>()) {
			assert(!m_isSerializer);
			auto& e = m_objects.at(key);
			ByteReaderImpl reader{ e.DataPtr, e.DataPtr + e.Length };
			size_t count = reader.Read<uint16_t, Endian>();
			for (size_t i = 0; i < count; ++i) {
				T result;
				converter.Deserialize(reader, result);
				container.emplace_back(result);
			}
		}

		template<typename T>
		void Get<T, std::vector<T>>(const std::string& key, std::vector<T>& container, IBinaryConverter<T>& converter = BinaryConverter<T>()) {
			assert(!m_isSerializer);
			auto& e = m_objects.at(key);
			ByteReaderImpl reader{ e.DataPtr, e.DataPtr + e.Length };
			size_t count = reader.Read<uint16_t, Endian>();
			container.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				T result;
				converter.Deserialize(reader, result);
				container.emplace_back(result);
			}
		}



		virtual void Serialize(IByteWriter& writer) override {
			assert(m_isSerializer);
			for (auto& e : m_objects) {
				size_t size = 5 + e.first.size() + e.second.Length;
				assert(size <= sizeof(uint32_t));
				writer.Write<uint32_t, Endian>(size);
				assert(e.first.size() <= sizeof(uint8_t));
				writer.Write<uint8_t>(e.first.size());
				writer.WriteAll(reinterpret_cast<const uint8_t*>(e.first.data()), e.first.size());
				writer.WriteAll(e.second.DataPtr, e.second.Length);
			}
		}

		template<size_t MaxAllocationSize = 65536>
		virtual void Deserialize(IByteReader& reader) override {
			assert(m_objects.empty());
			m_isSerializer = false;

			uint8_t* keyBuf = m_allocator.allocate(10);
			uint8_t keyBufSize = 10;

			while (reader.Available() > 0) {
				uint32_t size = reader.Read<uint32_t, Endian>();
				uint8_t keySize = reader.Read<uint8_t, Endian>();
				if (keyBufSize < keySize) {
					m_allocator.deallocate(keyBuf, keyBufSize);
					assert(keySize <= MaxAllocationSize);
					keyBuf = m_allocator.allocate(keySize);
					keyBufSize = keySize;
				}
				reader.ReadAll(keyBuf, keySize);
				std::string key{ reinterpret_cast<char*>(keyBuf), static_cast<size_t>(keySize) };

				uint32_t dataSize = size - (keySize + 5);
				assert(dataSize <= MaxAllocationSize);
				uint8_t* dataBuf = m_allocator.allocate(dataSize);
				reader.ReadAll(dataBuf, dataSize);

				m_objects.emplace(std::move(key), ContainerType(dataSize, dataBuf));
			}

			m_allocator.deallocate(keyBuf, keyBufSize);
		}

	};
}