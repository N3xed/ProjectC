#include "Config.h"

ProjectC::Config::Config() : m_tree(), m_currentTree(&m_tree)
{
}

ProjectC::Config::Config(const std::string& jsonText) : m_tree(), m_currentTree(&m_tree)
{
	std::stringstream ss{ jsonText };
	boost::property_tree::json_parser::read_json(ss, m_tree);
}

ProjectC::Config::Config(const UnicodeString& jsonText) : m_tree(), m_currentTree(&m_tree)
{
	std::stringstream ss{ StringUtils::ToUTF8(jsonText) };
	boost::property_tree::json_parser::read_json(ss, m_tree);
}

ProjectC::Config::Config(boost::property_tree::ptree tree) : m_tree(tree), m_currentTree(&m_tree)
{ }

void ProjectC::Config::Scope(const std::string& path)
{
	m_currentTree = &m_tree.get_child(path);
}

void ProjectC::Config::Unscope()
{
	m_currentTree = &m_tree;
}

void ProjectC::Config::Scope(const UnicodeString& path)
{
	m_currentTree = &m_tree.get_child(StringUtils::ToUTF8(path));
}

ProjectC::Config ProjectC::Config::GetDefault()
{
	return Config();
}
