#pragma once

#include "../UniString.h"
#include <Windows.h>
//#include <wx/msw/winundef.h>
#include <cassert>

namespace ProjectC {
	namespace Modules {

		class DynamicLibrary {
		private:
			HMODULE m_libraryHandle;
			UniString m_name;
		public:
			DynamicLibrary(const DynamicLibrary& obj) = delete;
			DynamicLibrary(DynamicLibrary&& obj);

			DynamicLibrary(const UniString& filePath);
			~DynamicLibrary();

			void* GetFunction(const UniString& name) const noexcept;
			const UniString& GetName() const noexcept { return m_name; }

			DynamicLibrary& operator=(DynamicLibrary&& obj) = delete;
			DynamicLibrary& operator=(const DynamicLibrary& obj) = delete;
		};
	}
}