#pragma once
#include <cef/include/internal/cef_string.h>
#include <sstream>

namespace ProjectC {
	typedef CefString UniString;

	typedef std::basic_string<CefString::char_type> BasicUniString;
}

namespace std {
	template<> struct hash<ProjectC::UniString> {
		typedef ProjectC::UniString argument_type;
		typedef std::size_t result_type;

		result_type operator()(const argument_type& key) const {
			return std::hash<std::wstring>{}(key.ToWString());
		}
	};
}