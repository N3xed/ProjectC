#include "ExecutionManager.h"

ProjectC::Modules::ExecutionContext& ProjectC::Modules::ExecutionManager::Create(const IModule::UID_t& uid)
{
	m_contexts.emplace_back(std::make_unique<ExecutionContext>(GetApp().GetModuleManager().GetModuleInfo(uid), static_cast<uint16_t>(m_contexts.size())));
	return *m_contexts.back();
}

ProjectC::Modules::ExecutionContext& ProjectC::Modules::ExecutionManager::Create(ModuleInfo& info)
{
	m_contexts.emplace_back(std::make_unique<ExecutionContext>(info, static_cast<uint16_t>(m_contexts.size())));
	return *m_contexts.back();
}

ProjectC::Modules::ExecutionContext& ProjectC::Modules::ExecutionManager::GetContext(uint16_t id)
{
	assert(id < m_contexts.size());
	return *m_contexts[id];
}

ProjectC::Modules::ExecutionContext* ProjectC::Modules::ExecutionManager::GetContext(const IModule::UID_t& uid)
{
	for (auto& e : m_contexts) {
		if (Utils::CompareByteArrays(e->GetModule().GetUID(), uid)) {
			return e.get();
		}
	}
	return nullptr;
}

ProjectC::Modules::ExecutionContext& ProjectC::Modules::ExecutionManager::GetContext(IModule& module)
{
	for (auto& e : m_contexts) {
		if (&(e->GetModule()) == static_cast<const IModule*>(&module)) {
			return *e;
		}
	}
	assert(false);
	throw std::logic_error("Must find module.");
}

ProjectC::Modules::ExecutionContext& ProjectC::Modules::ExecutionManager::GetContext(ModuleInfo& info)
{
	for (auto& e : m_contexts) {
		if (&(e->GetModuleInfo()) == static_cast<const ModuleInfo*>(&info)) {
			return *e;
		}
	}
	assert(false);
	throw std::logic_error("Must find module.");
}

