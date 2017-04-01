#pragma once
#include "../../BrowserWindow.h"
#include "WindowWin.h"

namespace ProjectC::Interface::Detail {
	class BrowserWindowWin : public BrowserWindow {
	private: 
		CefRefPtr<CefBrowser> m_browser;
		std::unique_ptr<WindowWin> m_window;
		
		BrowserWindowWin(WindowWin* window);
	public:
		virtual CefBrowser& GetBrowser() override { return *m_browser; }
		virtual IWindow& GetWindow() override { return *m_window; }
		
	public:
		static BrowserWindowWin* Create(const UniString& title, int32_t x, int32_t y, int32_t width, int32_t height, HINSTANCE hInstance, WindowHandle parent = nullptr, bool defaultPos = false);

		static BrowserWindowWin* FromNativeHandle(WindowHandle handle) {
			return reinterpret_cast<BrowserWindowWin*>(GetWindowLongPtr(handle, GWLP_USERDATA));
		}
	};
}