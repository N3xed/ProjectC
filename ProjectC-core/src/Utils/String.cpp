#include "String.h"

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const wchar_t* str)
{
	os << ProjectC::UniString{ str }.ToString();
	return os;
}

std::basic_ostream<ProjectC::UniString::char_type>& operator<<(std::basic_ostream<ProjectC::UniString::char_type>& os, const ProjectC::UniString& str)
{
	os << str.c_str();
	return os;
}

