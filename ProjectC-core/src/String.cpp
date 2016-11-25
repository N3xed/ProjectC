#include "String.h"



std::string ProjectC::StringUtils::ToUTF8(const UnicodeString& s) {
	std::string result;
	s.toUTF8String(result);
	return result;
}

std::string ProjectC::StringUtils::ToUTF8(const std::string& s) {
	std::string result;
	UnicodeString(s.c_str()).toUTF8String(result);
	return result;
}
