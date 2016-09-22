#pragma once

#include <stdint.h>
#include <memory>

namespace ProjectC {
	namespace Networking {
		template<typename T>
		class Container {
		public:
			size_t Length;
			T* DataPtr;

			Container(size_t length, T* ptr) : Length(length), DataPtr(ptr)
			{}
			Container() : Length(), DataPtr(nullptr)
			{}
		};

		template<typename T>
		class UniqueContainer {
		public:
			size_t Length;
			std::unique_ptr<T> DataPtr;

			UniqueContainer(size_t length, T* ptr) : Length(length), DataPtr(ptr)
			{}
			UniqueContainer() : Length(), DataPtr(nullptr)
			{}
		};

		template<typename T>
		class SharedContainer {
		public:
			size_t Length;
			std::shared_ptr<T> DataPtr;

			SharedContainer(size_t length, std::shared_ptr<T> ptr) : Length(length), DataPtr(ptr)
			{}
			SharedContainer() : Length(), DataPtr(nullptr)
			{}
		};

		typedef Container<const uint8_t> Buffer;
		typedef SharedContainer<uint8_t[]> SharedBuffer;
		typedef UniqueContainer<uint8_t[]> UniqueBuffer;
	}
}