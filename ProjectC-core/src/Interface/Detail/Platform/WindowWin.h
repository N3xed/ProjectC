#pragma once

#include <unordered_map>

#include "../../../UniString.h"
#include "../../Window.h"

namespace ProjectC::Interface::Detail {
	class WindowManagerWin;
	class BrowserWindowWin;

	class WindowWin : public Window {
		friend WindowManagerWin;
	public:
		WindowWin(CreatedCallback callback);

		virtual void CenterOnScreen() override;
		virtual void CenterOnParent() override;
		virtual void CenterOn(Window& window) override;

		virtual void SetFullscreenMode(FullscreenMode mode) override;
		virtual void SetInputCapture(bool capture) override;


		virtual FullscreenMode GetFullscreenMode() const override;
		virtual MonitorInfo GetMonitorInfo() const override;

		virtual bool IsInputCaptured() const override;

	
	};
}
