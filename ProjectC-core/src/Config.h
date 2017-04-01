#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Utils/String.h"
#include <sstream>
#include <stack>
#include "ConfigKeys.h"
#include "FactoryManager.h"

namespace ProjectC {

	class IConfig {
	public:
		~IConfig() {};

		template<typename Value>
		void Put(const std::string& path, const Value& value) {
			GetTree().put(path, value);
		}

		/// <summary>
		/// Gets the value as type.
		/// </summary>
		/// <param name="path">The path: one or more ids seperated by dots ("someThing.someChild.someValue").</param>
		/// <returns>The value identified by <c>path</c> converted to the specified type.</returns>
		template<typename Result>
		Result Get(const std::string& path) {
			return GetTree().get<Result>(path);
		}

		/// <summary>
		/// Tries to get the value as type.
		/// </summary>
		/// <param name="path">The path: one or more ids seperated by dots (ex. "someThing.someChild.someValue").</param>
		/// <returns><c>true</c> if there exists a value identified by <c>path</c>, <c>false</c> otherwise.</returns>
		template<typename Result> bool TryGet(const std::string& path, Result& result) {
			auto ref = m_stack.top()->get_child_optional(path);
			if (ref) {
				result = ref->get_value<Result>();
				return true;
			}
			return false;
		}

		/// <summary>
		/// Gets the value as type.
		/// </summary>
		/// <param name="path">The path: one or more ids seperated by dots (ex. "someThing.someChild.someValue").</param>
		/// <param name="def">The default value.</param>
		/// <returns>The value identified by <c>path</c>, or if path is invalid, the default specified by <c>def</c>.</returns>
		template<typename Result>
		Result GetOrDefault(const std::string& path, Result&& def) {
			return GetTree().get<Result>(path, def);
		}

		/// <summary>
		/// Gets the underlying tree.
		/// Note: This method could change rapidly.
		/// </summary>
		/// <returns>The underlying tree.</returns>
		virtual inline boost::property_tree::ptree& GetTree() = 0;
	};

	class Config : public IConfig {
	private:
		boost::property_tree::ptree m_tree;
	public:
		Config();
		Config(boost::property_tree::ptree tree);
		Config(const std::string& jsonText);

		inline virtual boost::property_tree::ptree& GetTree() override {
			return m_tree;
		}

	};

	class ScopedConfig : public IConfig {
		boost::property_tree::ptree* m_tree;
		bool m_isValid;
	public:
		ScopedConfig(IConfig& conf, const std::string& str, bool shouldCreate = true);
		ScopedConfig(boost::property_tree::ptree& tree);
		ScopedConfig();

		inline virtual boost::property_tree::ptree& GetTree() override {
			assert(m_tree != nullptr);
			return *m_tree;
		}
		inline void SetTree(boost::property_tree::ptree& tree) {
			m_isValid = true;
			m_tree = &tree;
		}
		inline bool IsValid() const {
			return m_isValid;
		}

		inline operator bool() {
			return m_isValid;
		}
	};
}