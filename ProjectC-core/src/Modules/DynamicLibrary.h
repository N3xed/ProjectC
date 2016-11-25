#pragma once

#include "../String.h"
#include <Windows.h>
//#include <wx/msw/winundef.h>
#include <cassert>

namespace ProjectC {
	namespace Modules {

		class DynamicLibrary {
		private:
			HMODULE m_libraryHandle;
			std::string m_name;
		public:
			DynamicLibrary(const DynamicLibrary& obj) = delete;
			DynamicLibrary(DynamicLibrary&& obj);

			DynamicLibrary(const std::string& filePath);
			DynamicLibrary(UnicodeString& filePath);
			~DynamicLibrary();

			void* GetFunction(std::string name) const noexcept;
			const std::string& GetName() const noexcept;

			DynamicLibrary& operator=(DynamicLibrary&& obj) = delete;
			DynamicLibrary& operator=(const DynamicLibrary& obj) = delete;
		};
	}
}