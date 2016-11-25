#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "String.h"
#include <sstream>
#include "ConfigKeys.h"

namespace ProjectC {
	class Config {
	private:
		boost::property_tree::ptree m_tree;
		boost::property_tree::ptree* m_currentTree;
	public:
		Config();
		Config(boost::property_tree::ptree tree);
		Config(const UnicodeString& jsonText);
		Config(const std::string& jsonText);

		template<typename Value>
		void Put(const std::string& path, const Value& value) {
			m_currentTree->put(path, value);
		}
		template<> void Put(const std::string& path, const UnicodeString& value) {
			m_currentTree->put(path, StringUtils::ToUTF8(value));
		}

		template<typename Result>
		Result Get(const std::string& path) {
			return m_currentTree->get<Result>(path);
		}
		template<> UnicodeString Get(const std::string& path) {
			return UnicodeString::fromUTF8(m_currentTree->get<std::string>(path));
		}

		template<typename Result>
		Result GetOrDefault(const std::string& path, Result&& def) {
			return m_currentTree->get<Result>(path, def);
		}
		template<> UnicodeString GetOrDefault(const std::string& path, UnicodeString&& def) {
			try {
				return UnicodeString::fromUTF8(m_currentTree->get<std::string>(path));
			}
			catch (boost::property_tree::ptree_error&) {
				return def;
			}
		}

		void Scope(const std::string& path);
		void Scope(const UnicodeString& path);
		void Unscope();

		boost::property_tree::ptree& GetTree() {
			return m_tree;
		}

		static Config GetDefault();
	};

	Config& GetConfig();
}