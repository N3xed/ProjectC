#pragma once

#include "App.h"
#include "Config.h"
#include "Networking/ConnectionManager.h"
#include "Modules/ModuleManager.h"
#include "Modules/ExecutionManager.h"
#include "Modules/LibraryManager.h"
#include "Logging.h"
#include "Interface/Localization/LanguageHelper.h"
#include "Interface/GUIContext.h"

namespace ProjectC {
	class Application : public IApplication {
	private:
		Config m_config;
		Modules::ModuleManager m_moduleMgr;
		Modules::ExecutionManager m_executionMgr;
		Modules::LibraryManager m_libraryMgr;
		Networking::ConnectionManager m_connectionMgr;
		Interface::LanguageHelper m_langHelper;

		static Application* s_instance;
	public:
		Application(CefMainArgs& args);
		virtual ~Application();

		virtual IConfig& GetConfig() override {
			return m_config;
		}
		virtual Networking::ConnectionManager& GetConnectionManager() override {
			return m_connectionMgr;
		}
		virtual Modules::ModuleManager& GetModuleManager() override {
			return m_moduleMgr;
		}
		virtual Modules::LibraryManager& GetLibraryManager() override {
			return m_libraryMgr;
		}
		virtual Modules::ExecutionManager& GetExecutionManager() override {
			return m_executionMgr;
		}
		virtual Interface::LanguageHelper& GetLanguageHelper() override {
			return m_langHelper;
		}

		static Application& GetInstance() {
			return *s_instance;
		}
		virtual void Exit(bool force = false) override;
	};
}