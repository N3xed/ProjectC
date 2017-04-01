#include "Config.h"

ProjectC::Config::Config() : m_tree()
{
}

ProjectC::Config::Config(const std::string& jsonText) : m_tree()
{
	std::stringstream ss{ jsonText };
	boost::property_tree::json_parser::read_json(ss, m_tree);
}

ProjectC::Config::Config(boost::property_tree::ptree tree) : m_tree(tree)
{ 
}

ProjectC::ScopedConfig::ScopedConfig(IConfig& conf, const std::string& str, bool shouldCreate) : m_isValid(true)
{
	auto optTree = conf.GetTree().get_child_optional(str);
	if (optTree) {
		m_tree = optTree.get_ptr();
	}
	else if(shouldCreate) {
		m_tree = &conf.GetTree().put_child(str, boost::property_tree::ptree{});
	}
	else {
		m_isValid = false;
	}
}

ProjectC::ScopedConfig::ScopedConfig() : m_tree(nullptr), m_isValid(false)
{}

ProjectC::ScopedConfig::ScopedConfig(boost::property_tree::ptree& tree) : m_tree(&tree), m_isValid(true)
{}
