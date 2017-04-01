#include "BrowserWindowWin.h"
#include "BrowserHandlerWin.h"


ProjectC::Interface::Detail::BrowserWindowWin::BrowserWindowWin(WindowWin* window) : m_window(window)
{
	SetWindowLongPtr(m_window->GetNativeHandle(), GWLP_USERDATA, reinterpret_cast<uintptr_t>(this));

	Size clientSize = window->GetClientSize();
	CefWindowInfo wndInfo;
	RECT rect{
		0,
		0,
		clientSize.x,
		clientSize.y
	};

	wndInfo.SetAsChild(window->GetNativeHandle(), rect);

	CefBrowserSettings browserSettings;
	browserSettings.plugins = STATE_DISABLED;
	browserSettings.web_security = STATE_DISABLED;

	m_browser = CefBrowserHost::CreateBrowserSync(wndInfo, new BrowserHandlerWin(this), "", browserSettings, nullptr);
}

ProjectC::Interface::Detail::BrowserWindowWin* ProjectC::Interface::Detail::BrowserWindowWin::Create(const UniString& title, int32_t x, int32_t y, int32_t width, int32_t height, HINSTANCE hInstance, WindowHandle parent /*= nullptr*/, bool defaultPos /*= false*/)
{
	if (auto result = new BrowserWindowWin(WindowWin::Create(title, x, y, width, height, hInstance, parent, defaultPos))) {
		return result;
	}
	else {
		delete result;
		return nullptr;
	}
}
