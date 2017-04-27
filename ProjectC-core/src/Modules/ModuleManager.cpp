#include "ModuleManager.h"
#include "DynamicLibrary.h"
#include "../Logging.h"

namespace pm = ProjectC::Modules;


ProjectC::Modules::IModule& ProjectC::Modules::ModuleManager::GetModule(const IModule::UID_t& uid)
{
	return GetModuleInfo(uid).GetModule();
}

ProjectC::Modules::IModule& ProjectC::Modules::ModuleManager::GetModule(size_t index)
{
	return GetModuleInfo(index).GetModule();
}

size_t ProjectC::Modules::ModuleManager::GetIndex(const IModule::UID_t& uid)
{
	auto end = m_modules.end();
	for (auto it = m_modules.begin(); it != end; ++it) {
		IModule& module = it->GetModule();
		if (Utils::CompareByteArrays(module.GetUID(), uid)) {
			return it - m_modules.begin();
		}
	}
	throw std::exception("Module not found.");
}

size_t ProjectC::Modules::ModuleManager::GetIndex(const IModule& module)
{
	auto end = m_modules.end();
	for (auto it = m_modules.begin(); it != end; ++it) {
		if ((const IModule*)&it->GetModule() == &module) {
			return it - m_modules.begin();
		}
	}
	throw std::exception("Module not found.");
}

size_t ProjectC::Modules::ModuleManager::GetCount()
{
	return m_modules.size();
}

ProjectC::Modules::IModule::RoutineRegisterer ProjectC::Modules::ModuleManager::GetRegisterer(IModule* module, ModuleInfo& info)
{
	return [module, &info](int16_t id, IModule::Routine routine) {
		if (!info.AddRoutine(id, routine)) {
			PROJC_LOG(FATAL, "Could not add routine ", info.GetLibrary()->GetName(), "/", info.GetModule().GetName(), "/", id);
		}
	};
}

const ProjectC::Modules::ModuleInfo& ProjectC::Modules::ModuleManager::GetModuleInfo(const IModule::UID_t& uid)
{
	for (auto& e : m_modules) {
		IModule& module = e.GetModule();
		if (Utils::CompareByteArrays(module.GetUID(), uid)) {
			return e;
		}
	}
	throw std::exception("Module not found.");
}

const ProjectC::Modules::ModuleInfo& ProjectC::Modules::ModuleManager::GetModuleInfo(const IModule& module)
{
	for (auto& e : m_modules) {
		if ((const IModule*)&e.GetModule() == &module)
			return e;
	}
	throw std::exception("Module not found.");
}

const ProjectC::Modules::ModuleInfo& ProjectC::Modules::ModuleManager::GetModuleInfo(size_t index)
{
	assert(index < m_modules.size());
	return m_modules[index];
}

const ProjectC::Modules::ModuleInfo& ProjectC::Modules::ModuleManager::AddModule(DynamicLibrary& library, ModuleFactory factory)
{
	auto module = factory();
	m_modules.emplace_back(module, &library);
	module->RegisterRoutines(GetRegisterer(module, m_modules.back()));
	return m_modules.back();
}

const ProjectC::Modules::ModuleInfo& ProjectC::Modules::ModuleManager::AddModule(ModuleFactory factory)
{
	auto module = factory();
	m_modules.emplace_back(module, nullptr);
	module->RegisterRoutines(GetRegisterer(module, m_modules.back()));
	return m_modules.back();
}

const ProjectC::Modules::ModuleInfo& ProjectC::Modules::ModuleManager::AddModule(DynamicLibrary& library, const std::string& factoryName)
{
	void* factoryFuncRaw = library.GetFunction(factoryName);
	if (factoryFuncRaw == nullptr)
		throw std::exception(("Could not find function '" + factoryName + "' from library: "/* + StringUtils::ToUTF8(library.GetName())*/).c_str());
	ModuleFactoryPtr ptr = (ModuleFactoryPtr)factoryFuncRaw;
	auto module = ptr();
	m_modules.emplace_back(module, &library);
	module->RegisterRoutines(GetRegisterer(module, m_modules.back()));
	return m_modules.back();
}

ProjectC::Modules::ModuleInfo::ModuleInfo(IModule* module, DynamicLibrary* library /*= nullptr*/) : m_module(module), m_library(library)
{ }

bool ProjectC::Modules::ModuleInfo::AddRoutine(int16_t id, IModule::Routine routine)
{
	auto result = m_routines.emplace(id, routine);
	return result.second;
}
