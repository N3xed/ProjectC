#pragma once
#include "../../WindowManager.h"
#include "../../../Platform.h"
#include "BrowserWindowWin.h"

namespace ProjectC::Interface::Detail {
	class WindowManagerWin : public WindowManager {
	public:
		static const wchar_t WINDOW_CLASS_NAME[];
		static ATOM WINDOW_CLASS;

		virtual std::shared_ptr<BrowserWindow> DoGetWindow(WindowHandle handle) override;

	protected:
		virtual ~WindowManagerWin();

		virtual bool PlatformInitialize() override;

	private:
		static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}