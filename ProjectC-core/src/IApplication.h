#pragma once

#include <vector>
#include <cstdint>
#include <functional>

namespace ProjectC {
	class Config;
}
namespace ProjectC::Networking {
	class ConnectionManager;
}
namespace ProjectC::Modules {
	class IModule;
	class ModuleManager;
	class LibraryManager;
	class ExecutionManager;
}
namespace ProjectC::Interface {
	class IGUIModule;
	class IFrame;
	class LanguageHelper;
	class LanguageHelper;
}

namespace ProjectC {
	const enum class LogType: uint8_t;

	class IApplication {
	public:
		virtual void ShowNewFrame(Interface::IFrame& from, Interface::IGUIModule* guiModule = nullptr, Modules::IModule* module = nullptr) = 0;
		virtual void ShowNewFrame(Interface::IGUIModule* guiModule = nullptr, Modules::IModule* module = nullptr) = 0;
		virtual void ShowLayer(Interface::IFrame& frame, Interface::IGUIModule* guiModule, Modules::IModule* module = nullptr) = 0;
		virtual void ShowLayerOrRaiseFrame(bool newFrame, Interface::IFrame& frame, Interface::IGUIModule* guiModule, Modules::IModule* module = nullptr) = 0;

		virtual std::vector<Interface::IFrame*> GetFrames() const = 0;

		virtual void Exit(std::string msg = "") = 0;

		virtual void RunOnUIThread(std::function<void()> func) = 0;
		virtual Interface::LanguageHelper& GetLanguageHelper() = 0;

		virtual Config& GetConfig() = 0;
		virtual Networking::ConnectionManager& GetConnectionManager() = 0;
		virtual Modules::ModuleManager& GetModuleManager() = 0;
		virtual Modules::LibraryManager& GetLibraryManager() = 0;
		virtual Modules::ExecutionManager& GetExecutionManager() = 0;
		virtual size_t GetFramesCount() = 0;

		virtual void Log(const std::string& msg, LogType type = static_cast<LogType>(0)) = 0;
	};
}