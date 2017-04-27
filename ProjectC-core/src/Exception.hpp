#pragma once
#include "UniString.h"

namespace ProjectC {
	class Exception : std::exception {
	private:
		std::string m_message;
	public:
		Exception(const UniString& str) : m_message(str) {}
		Exception(std::string str) : m_message(str) {}

		virtual char const* what() const override
		{
			return m_message.c_str();
		}
	};
}