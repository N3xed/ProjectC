#include "String.h"

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const wchar_t* str)
{
	os << ProjectC::UniString{ str }.ToString();
	return os;
}

