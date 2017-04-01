#pragma once

#include "../UniString.h"
#include "DynamicLibrary.h"
#include <forward_list>

namespace ProjectC {
	namespace Modules {
		class LibraryManager {
		private:
			std::forward_list<DynamicLibrary> m_libraries;
		
		public:

			DynamicLibrary& AddLibrary(DynamicLibrary&& library);
			DynamicLibrary& AddLibrary(const UniString& path);

			bool RemoveLibrary(const std::string& name);
			bool RemoveLibrary(const DynamicLibrary& library);

			const std::forward_list<DynamicLibrary>& GetLibraries() const noexcept;

		};
	}
}