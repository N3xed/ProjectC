#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include "Utils/String.h"

namespace ProjectC {
	class Application;

	class IConfig;
	class QueueLogger;
}
namespace ProjectC::Networking {
	class ConnectionManager;
}
namespace ProjectC::Modules {
	class ModuleManager;
	class LibraryManager;
	class ExecutionManager;
}
namespace ProjectC::Interface {
	class LanguageHelper;
	class WindowManager;
	class GUIContext;
}

namespace ProjectC {
	const enum class ExitCode : int32_t {
		INITIALIZATION_FAILURE = -1,
		NONE = 0,
		PROCESS_EXIT = 1
	};

	class IApplication {
	public:
		virtual ~IApplication() {}

		virtual Interface::LanguageHelper& GetLanguageHelper() = 0;

		virtual IConfig& GetConfig() = 0;
		virtual Networking::ConnectionManager& GetConnectionManager() = 0;
		virtual Modules::ModuleManager& GetModuleManager() = 0;
		virtual Modules::LibraryManager& GetLibraryManager() = 0;
		virtual Modules::ExecutionManager& GetExecutionManager() = 0;

		virtual void Exit(bool force = false) = 0;

	};
}

namespace ProjectC {
	namespace App {
		IApplication& Inst();

		inline IConfig& Conf() {
			return Inst().GetConfig();
		}
		inline Interface::LanguageHelper& LangHelper() {
			return Inst().GetLanguageHelper();
		}
		inline Networking::ConnectionManager& ConMgr() {
			return Inst().GetConnectionManager();
		}
		inline Modules::ModuleManager& ModuleMgr() {
			return Inst().GetModuleManager();
		}
		inline Modules::LibraryManager& LibMgr() {
			return Inst().GetLibraryManager();
		}
		inline Modules::ExecutionManager& ExecMgr() {
			return Inst().GetExecutionManager();
		}
		Interface::WindowManager& WndMgr();
		Interface::GUIContext& GUICnxt();
	}
}