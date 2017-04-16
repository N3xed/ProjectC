#pragma once

#include <string>
#include <sstream>
#include "../UniString.h"
#include <vector>

namespace ProjectC {
	namespace StringUtils {

		template<typename CharType = char, typename T, typename... Args>
		static void Concatenate(std::basic_stringstream<CharType>& ss, const T& arg0, const Args&... args) {
			ss << arg0;
			Concatenate<CharType>(ss, args...);
		}
		
		template<typename CharType = char, typename T>
		static void Concatenate(std::basic_stringstream<CharType>& ss, const T& arg) {
			ss << arg;
		}
		
		template<typename CharType = char, typename... Args>
		static std::basic_string<CharType> Concatenate(Args... args) {
			std::basic_stringstream<CharType> ss{};
			Concatenate<CharType>(ss, args...);
			return ss.str();
		}
		
		template<typename CharType = char, typename... Args>
		static std::basic_string<CharType> Append(const std::basic_string<CharType>& str, const Args&... args) {
			std::basic_stringstream<CharType> ss{};
			ss << str;
			Concatenate<CharType>(ss, args...);
			return ss.str();
		}

		template<typename CharType> static std::wstring ToWString(const CharType* str, size_t length) {}
		template<> static std::wstring ToWString(const char* str, size_t length) {
			return UniString{ std::string{ str, length } }.ToWString();
		}
		template<> static std::wstring ToWString(const wchar_t* str, size_t length) {
			return std::wstring{ str, length };
		}
		template<typename CharType> static std::string ToString(const CharType* str, size_t length) {}
		template<> static std::string ToString(const char* str, size_t length) {
			return std::string{ str, length };
		}
		template<> static std::string ToString(const wchar_t* str, size_t length) {
			return UniString{ str, length, true }.ToString();
		}
		template<typename CharType> static UniString ToUniString(const CharType* str, size_t length) {}
		template<> static UniString ToUniString(const char* str, size_t length) {
			return UniString{ std::string{str, length} };
		}
		template<> static UniString ToUniString(const wchar_t* str, size_t length) {
			return UniString{ str, length, true };
		}
#if defined(WCHAR_T_IS_UTF32)
		template<> static std::wstring ToWString(const UniString::char_type* str, size_t length) {
			return UniString{ str, length, true }.ToWString();
		}
		template<> static std::string ToString(const UniString::char_type* str, size_t length) {
			return UniString{ str, length, true }.ToString();
		}
		template<> static UniString ToUniString(const UniString::char_type* str, size_t length) {
			return UniString{ str, length, true };
		}
#endif

		template<typename CharType>
		class SubStringInfo {
		public:
			const CharType* String;
			const size_t Length;

			SubStringInfo(const CharType* str, const size_t length) : String(str), Length(length)
			{}

			std::string ToString() const {
				return StringUtils::ToString(String, Length);
			}
			std::wstring ToWString() const {
				return StringUtils::ToWString(String, Length);
			}
			UniString ToUniString() const {
				return StringUtils::ToUniString(String, Length);
			}
			operator std::string() const {
				return StringUtils::ToString(String, Length);
			}
			operator std::wstring() const {
				return StringUtils::ToWString(String, Length);
			}
			operator UniString() const {
				return StringUtils::ToUniString(String, Length);
			}
		};

		template<typename CharType>
		SubStringInfo<CharType> SubStringUntil(const std::basic_string<CharType>& str, const std::basic_string<CharType>& del, size_t offset = 0) {
			size_t pos;
			if ((pos = str.find(del, offset)) == std::string::npos) {
				return SubStringInfo<CharType>(nullptr, 0);
			}
			else {
				return SubStringInfo<CharType>(str.data() + offset, pos - offset);
			}
		}

		template<typename CharType>
		SubStringInfo<UniString::char_type> SubStringUntil(const UniString& srcStr, std::basic_string<CharType>& destStr, const UniString& del, size_t offset = 0) {
			destStr = srcStr;
			return SubStringUntil<UniString::char_type>(destStr, del, offset);
		}

		template<typename CharType>
		SubStringInfo<CharType> SubStringAfterFirst(const std::basic_string<CharType>& str, const std::basic_string<CharType>& del, size_t offset = 0) {
			size_t pos;
			if ((pos = str.find(del, offset)) == std::string::npos) {
				return SubStringInfo<CharType>(nullptr, 0);
			}
			else {
				return SubStringInfo<CharType>(str.data() + pos + del.length(), str.length() - (pos + del.length()));
			}
		}

		template<typename CharType>
		SubStringInfo<UniString::char_type> SubStringAfterFirst(const UniString& srcStr, std::basic_string<CharType>& destStr, const UniString& del, size_t offset = 0) {
			destStr = srcStr;
			return SubStringAfterFirst<UniString::char_type>(destStr, del, offset);
		}

		template<typename CharType>
		SubStringInfo<CharType> SubStringAfterLast(const std::basic_string<CharType>& str, const std::basic_string<CharType>& del, size_t offset) {
			offset = str.length() - offset - 1;
			size_t pos = 0;
			if ((pos = str.find_last_of(del, offset)) == std::string::npos) {
				return SubStringInfo<CharType>(nullptr, 0);
			}
			else {
				return SubStringInfo<CharType>(str.data() + pos + del.length(), str.length() - (pos + del.length()));
			}
		}

		template<typename CharType>
		SubStringInfo<UniString::char_type> SubStringAfterLast(const UniString& srcStr, std::basic_string<CharType>& destStr, const UniString& del, size_t offset = 0) {
			destStr = srcStr;
			return SubStringAfterLast<UniString::char_type>(destStr, del, offset);
		}

		template<typename CharType>
		void SplitString(const std::basic_string<CharType>& str, const std::basic_string<CharType>& del, std::vector<SubStringInfo<CharType>>& vector, size_t offset = 0) {
			size_t pos = 0;
			while ((pos = str.find(del, offset)) != std::string::npos) {
				vector.emplace_back(SubStringInfo<CharType>{ str.data() + offset, pos - offset });
				offset = pos + del.length();
			}
			vector.emplace_back(SubStringInfo<CharType>{ str.data() + offset, str.length() - offset });
		}

		template<typename CharType>
		void SplitString(const UniString& srcStr, std::basic_string<CharType>& destStr, const UniString& del, std::vector<SubStringInfo<UniString::char_type>>& vector, size_t offset = 0) {
			destStr = srcStr;
			SplitString<UniString::char_type>(destStr, del, vector, offset);
		}

		template<typename CharType>
		std::vector<SubStringInfo<CharType>> SplitString(const std::basic_string<CharType>& str, const std::basic_string<CharType>& del, size_t offset = 0) {
			std::vector<SubStringInfo<CharType>> vector;
			SplitString(str, del, vector, offset);
			return vector;
		}

		template<typename CharType>
		std::vector<SubStringInfo<UniString::char_type>> SplitString(const UniString& srcStr, std::basic_string<CharType>& destStr, const UniString& del, size_t offset = 0) {
			destStr = srcStr;
			std::vector<SubStringInfo<UniString::char_type>> vector;
			SplitString(destStr, del, vector, offset);
			return vector;
		}
	}
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const wchar_t* str);
std::basic_ostream<ProjectC::UniString::char_type>& operator<<(std::basic_ostream<ProjectC::UniString::char_type>& os, const ProjectC::UniString& str);