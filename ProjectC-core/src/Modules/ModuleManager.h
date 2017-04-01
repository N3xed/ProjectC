#pragma once

#include "IModule.h"
#include <tuple>
#include <map>
#include <memory>
#include "../Utils/Utils.h"

namespace ProjectC {
	namespace Modules {

		class DynamicLibrary;

		class ModuleInfo {
		private:
			DynamicLibrary* m_library;
			std::unique_ptr<IModule> m_module;
			std::map<uint16_t, IModule::Routine> m_routines;
		public:
			ModuleInfo(IModule* module, DynamicLibrary* library = nullptr);

			DynamicLibrary* GetLibrary() const noexcept {
				return m_library;
			}
			IModule& GetModule() const noexcept {
				return *m_module;
			}
			IModule::Routine GetRoutine(uint16_t id) const {
				return m_routines.at(id);
			}
			size_t RoutineCount() const {
				return m_routines.size();
			}
			bool AddRoutine(int16_t id, IModule::Routine routine);
		};

		class ModuleManager {
		private:
			std::vector<ModuleInfo> m_modules;

		public:
			typedef IModule*(*ModuleFactoryPtr)();
			typedef std::function<IModule*()> ModuleFactory;

			const ModuleInfo& AddModule(DynamicLibrary& library, ModuleFactory factory);
			const ModuleInfo& AddModule(ModuleFactory factory);
			const ModuleInfo& AddModule(DynamicLibrary& library, const std::string& factoryName);

			IModule& GetModule(const IModule::UID_t& uid);
			IModule& GetModule(size_t index);
			const ModuleInfo& GetModuleInfo(const IModule::UID_t& uid);
			const ModuleInfo& GetModuleInfo(size_t index);
			const ModuleInfo& GetModuleInfo(const IModule& module);
			size_t GetIndex(const IModule::UID_t& uid);
			size_t GetIndex(const IModule& module);
			size_t GetCount();

		private:
			IModule::RoutineRegisterer GetRegisterer(IModule* module, ModuleInfo& info);

		};
	}
}