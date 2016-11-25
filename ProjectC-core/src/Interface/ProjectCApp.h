#pragma once

#include "../IApplication.h"
#include <wx/wx.h>
#include "ProjectCFrame.h"
#include "InvokeEvent.h"
#include "../Config.h"
#include "../Networking/ConnectionManager.h"
#include "../Modules/ModuleManager.h"
#include "../Modules/ExecutionManager.h"
#include "../Modules/LibraryManager.h"
#include "../Logger.h"
#include "ConsoleFrame.h"
#include "Localization/LanguageHelper.h"

namespace ProjectC {
	namespace Interface {

		class ProjectCApp : public wxApp, public IApplication {
		private:
			std::vector<ProjectCFrame*> m_frames;

			Config m_config;
			Modules::ModuleManager m_moduleMgr;
			Modules::ExecutionManager m_executionMgr;
			Modules::LibraryManager m_libraryMgr;
			Networking::ConnectionManager m_connectionMgr;
			ConsoleFrame* m_consoleFrame;
			QueueLogger m_logger;
			LanguageHelper m_langHelper;
		public:
			ProjectCApp();
			
			virtual void ShowNewFrame(IFrame& from, IGUIModule* guiModule = nullptr, Modules::IModule* module = nullptr) override;
			virtual void ShowNewFrame(Interface::IGUIModule* guiModule = nullptr, Modules::IModule* module = nullptr) override;
			virtual void ShowLayer(IFrame& frame, IGUIModule* guiModule, Modules::IModule* module = nullptr) override;
			virtual void ShowLayerOrRaiseFrame(bool newFrame, IFrame& frame, IGUIModule* guiModule, Modules::IModule* module = nullptr) override;

			virtual bool OnInit();
			virtual int OnExit();
			
			virtual std::vector<IFrame*> GetFrames() const override;
			virtual void Exit(std::string msg = "") override;

			virtual Config& GetConfig() override {
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
			ConsoleFrame& GetConsoleFrame() {
				return *m_consoleFrame;
			}
			virtual size_t GetFramesCount() override {
				return m_frames.size();
			}

			virtual void RunOnUIThread(std::function<void() > func) override;

			virtual Interface::LanguageHelper& GetLanguageHelper() override {
				return m_langHelper;
			}

			virtual void Log(const std::string& msg, LogType type = static_cast<LogType>(0)) override;

		private:
			void handleInvokeEvent(InvokeEvent& ev);
		};
	}
}