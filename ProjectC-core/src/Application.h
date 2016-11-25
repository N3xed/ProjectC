#pragma once

#include "IApplication.h"

namespace ProjectC {

	IApplication*& GetApplicationInstancePtr();
	Config*& GetConfigInstancePtr();

	IApplication& GetApp();

	namespace App {
		inline Config& Conf() {
			return GetApp().GetConfig();
		}
		inline Interface::LanguageHelper& LangHelper() {
			return GetApp().GetLanguageHelper();
		}
		inline Networking::ConnectionManager& ConMgr() {
			return GetApp().GetConnectionManager();
		}
		inline Modules::ModuleManager& ModuleMgr() {
			return GetApp().GetModuleManager();
		}
		inline Modules::LibraryManager& LibMgr() {
			return GetApp().GetLibraryManager();
		}
		inline Modules::ExecutionManager& ExecMgr() {
			return GetApp().GetExecutionManager();
		}
	}
}
