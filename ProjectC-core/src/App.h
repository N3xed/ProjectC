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
	const enum class LogType : uint8_t {
		NORMAL = 0,
		WARN = 1,
		FATAL = 2
	};

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
		virtual QueueLogger& GetQueueLogger() = 0;

		virtual void Exit(bool force = false) = 0;

		virtual void Log(const std::string& msg, LogType type = LogType::NORMAL) = 0;

		template<typename... Args>
		void Log(LogType type, Args const&... args) {
			Log(StringUtils::Concatenate(args...), type);
		}
	};
}

#define STRINGIZE_EX(X) #X
#define STRINGIZE(X) STRINGIZE_EX(X)

#define LOG_NORMAL(...) App::Inst().Log(LogType::NORMAL, "[" __FILE__ ":" STRINGIZE(__LINE__) "] ", __VA_ARGS__)
#define LOG_WARN(...) App::Inst().Log(LogType::WARN, "[" __FILE__ ":" STRINGIZE(__LINE__) "] ", __VA_ARGS__)
#define LOG_FATAL(...) App::Inst().Log(LogType::FATAL, "[" __FILE__ ":" STRINGIZE(__LINE__) "] ", __VA_ARGS__)

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