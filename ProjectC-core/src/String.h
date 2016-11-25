#pragma once

#include <unicode/unistr.h>
#include <string>

namespace ProjectC {
	typedef icu::UnicodeString UnicodeString;

	namespace StringUtils {
		std::string ToUTF8(const UnicodeString& s);

		std::string ToUTF8(const std::string& s);

		template<typename T, typename... Args>
		inline void Concatenate(std::stringstream& ss, const T& arg0, Args... args) {
			ss << arg0;
			Concatenate(ss, std::forward<Args>(args)...);
		}

		template<typename T>
		inline void Concatenate(std::stringstream& ss, const T& arg) {
			ss << arg;
		}

		template<typename... Args>
		std::string Concatenate(Args... args) {
			std::stringstream ss;
			Concatenate(ss, std::forward<Args>(args)...);
			return ss.str();
		}

		template<typename Sender, typename... Args>
		std::string GetErrorMessage(const std::string& causer, Args... args) {
			std::stringstream ss;
			ss << '[' << typeid(Sender).name() << "] Exception thrown by '" << causer << "':\n\t";
			Concatenate(ss, std::forward<Args>(args)...);
			return ss.str();
		}
	}
}