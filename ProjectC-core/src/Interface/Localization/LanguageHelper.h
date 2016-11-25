#pragma once

#include "String.h"
#include <vector>
#include "StringKeys.h"

namespace ProjectC::Interface {
	class LanguageHelper {
	private:
		std::vector<std::string> m_strings;

	public:
		LanguageHelper() {}
		LanguageHelper(std::initializer_list<std::string>&& list) : m_strings(list) {}
		LanguageHelper(std::vector<std::string>&& vec) : m_strings(vec) {}
		template<typename Iter> LanguageHelper(Iter begin, Iter end) : m_strings(begin, end) {}

		void Load(std::initializer_list<std::string>&& list) {
			m_strings.assign(list);
		}

		void Load(std::vector<std::string>&& vec) {
			m_strings = vec;
		}

		template<typename Iter> void Load(Iter begin, Iter end) {
			m_strings.assign(begin, end);
		}

		const std::string& operator[](uint32_t index) const {
			return m_strings[index];
		}

		const std::string& GetString(uint32_t index) const {
			return m_strings.at(index);
		}
	};

	namespace StringResources {
		extern std::initializer_list<std::string> English;
	}
}