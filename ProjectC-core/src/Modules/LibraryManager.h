#pragma once

#include "../String.h"
#include "DynamicLibrary.h"
#include <forward_list>

namespace ProjectC {
	namespace Modules {
		class LibraryManager {
		private:
			std::forward_list<DynamicLibrary> m_libraries;
		
		public:

			DynamicLibrary& AddLibrary(DynamicLibrary&& library);
			DynamicLibrary& AddLibrary(const std::string& path);
			DynamicLibrary& AddLibrary(UnicodeString& path);

			bool RemoveLibrary(const UnicodeString& name);
			bool RemoveLibrary(const std::string& name);
			bool RemoveLibrary(const DynamicLibrary& library);

			const std::forward_list<DynamicLibrary>& GetLibraries() const noexcept;

		};
	}
}