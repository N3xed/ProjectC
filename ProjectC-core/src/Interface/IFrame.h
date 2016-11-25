#pragma once

#include <tuple>
#include <string>

#include "IStatusBar.h"

class wxMenuBar;

namespace ProjectC::Modules {
	class IModule;
}

namespace ProjectC::Interface {
	class IGUIModule;

	class IFrame {
	public:
		virtual void PushLayer(IGUIModule* guiModule, Modules::IModule* module = nullptr) = 0;
		virtual void PopLayer() = 0;

		virtual void Hide() = 0;
		virtual void Show() = 0;

		virtual void BringToFront() = 0;

		virtual void SetMenuBar(wxMenuBar* menuBar) = 0;
		virtual void SetNavigationVisible(bool value) = 0;
		virtual void SetStatusBarVisible(bool value) = 0;

		virtual void PushStatus(const std::string& text) = 0;
		virtual IStatusBar& GetStatusBar() = 0;

		virtual bool IsNavigationVisible() const = 0;
		virtual bool IsStatusBarVisible() const = 0;
		virtual bool IsMainWindow() const = 0;
		virtual bool IsHomeScreen() const = 0;
		virtual std::tuple<IGUIModule*, Modules::IModule*> GetCurrentModule() const = 0;
	};
}