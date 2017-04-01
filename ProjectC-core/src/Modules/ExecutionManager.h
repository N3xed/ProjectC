#pragma once

#include "ExecutionContext.h"

namespace ProjectC::Modules {
	class ModuleManager;

	class ExecutionManager {
	private:
		std::vector<std::unique_ptr<ExecutionContext>> m_contexts;

	public:

		ExecutionContext& Create(const IModule::UID_t& uid);
		ExecutionContext& Create(ModuleInfo& info);

		ExecutionContext* GetContext(uint16_t id);
		ExecutionContext* GetContext(const IModule::UID_t& uid);
		ExecutionContext& GetContext(IModule& module);
		ExecutionContext& GetContext(ModuleInfo& info);

	};

}