#include <windows.h>

#include "WindowWin.h"
#include "WindowManagerWin.h"
#include "../../../App.h"
#include "../../../Utils/String.h"
#include "BrowserWindowWin.h"


ProjectC::Interface::Detail::WindowWin::WindowWin(WindowHandle handle) : m_handle(handle)
{ }

ProjectC::UniString ProjectC::Interface::Detail::WindowWin::GetLastErrorString()
{
	wchar_t* buffer;
	uint32_t errCode = GetLastError();
	uint32_t result = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, errCode, 0, (LPWSTR)&buffer, 0, nullptr);
	if (result == 0) {
		return StringUtils::Concatenate("Failed to get error message. (ERRCODE ", errCode, ")");
	}
	else {
		UniString result{ buffer };
		HeapFree(GetProcessHeap(), NULL, buffer);
		return StringUtils::Concatenate(result.ToString(), "(ERRCODE ", errCode, ")");
	}
}

ProjectC::Interface::Detail::WindowWin* ProjectC::Interface::Detail::WindowWin::Create(const UniString& title, int32_t x, int32_t y, int32_t width, int32_t height, HINSTANCE hInstance, WindowHandle parent /*= nullptr*/, bool defaultPos)
{
	WindowHandle handle = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		Detail::WindowManagerWin::WINDOW_CLASS_NAME,
		title.ToWString().c_str(),
		WS_CLIPCHILDREN | WS_THICKFRAME | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
		(defaultPos) ? CW_USEDEFAULT : x,
		(defaultPos) ? CW_USEDEFAULT : y,
		(width < 0) ? CW_USEDEFAULT : width,
		(height < 0) ? CW_USEDEFAULT : height,
		parent,
		NULL,
		hInstance,
		NULL
	);
	if (handle == NULL) {
		LOG_FATAL("CreateWindowEx failed: ", GetLastErrorString());
		return nullptr;
	}
	return new WindowWin(handle);
}

void ProjectC::Interface::Detail::WindowWin::Hide()
{
	Show(false);
}

void ProjectC::Interface::Detail::WindowWin::Show(bool show /*= true*/)
{
	switch (show) {
	case true:
		if (!ShowWindowAsync(m_handle, SW_SHOW))
			LOG_FATAL("ShowWindowAsync failed: ", GetLastErrorString());
		OnShowChanged()(*this, true);
		break;
	case false:
		if (!ShowWindowAsync(m_handle, SW_HIDE))
			LOG_FATAL("ShowWindowAsync failed: ", GetLastErrorString());
		OnShowChanged()(*this, false);
		break;
	}
}

void ProjectC::Interface::Detail::WindowWin::Raise()
{
	if (!BringWindowToTop(m_handle))
		LOG_FATAL("BringWindowToTop failed:", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::Mimimize()
{
	if (!ShowWindowAsync(m_handle, SW_MINIMIZE))
		LOG_FATAL("ShowWindowAsync failed: ", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::Maximize()
{
	if (!ShowWindowAsync(m_handle, SW_MAXIMIZE))
		LOG_FATAL("ShowWindowAsync failed: ", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::Restore()
{
	if (!ShowWindowAsync(m_handle, SW_RESTORE))
		LOG_FATAL("ShowWindowAsync failed: ", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::SetTitle(UniString const& title)
{
	if (!SetWindowText(m_handle, title.c_str()))
		LOG_FATAL("SetWindowText failed: ", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::CenterOnScreen()
{
	Rect rect = GetRect();
	MONITORINFO mtrInfo;
	mtrInfo.cbSize = sizeof(MONITORINFO);
	if (!::GetMonitorInfoW(MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST), &mtrInfo)) {
		LOG_FATAL("GetMonitorInfo failed: ", GetLastErrorString());
		return;
	}
	SetPosition(Point{
		mtrInfo.rcWork.left + (mtrInfo.rcWork.right - mtrInfo.rcWork.left - rect.width) / 2,
		mtrInfo.rcWork.top + (mtrInfo.rcWork.bottom - mtrInfo.rcWork.top - rect.height) / 2
	});
}

void ProjectC::Interface::Detail::WindowWin::CenterOnParent()
{
	HWND wndHandle = ::GetParent(m_handle);
	if (wndHandle == NULL) {
		LOG_FATAL("GetParent failed: ", GetLastErrorString());
		return;
	}
	RECT rect;
	if (!GetWindowRect(wndHandle, &rect)) {
		LOG_FATAL("GetWindowRect failed: ", GetLastErrorString());
		return;
	}
	Size size = GetSize();
	SetPosition(Point{
		rect.left + (rect.right - rect.left - size.x) / 2,
		rect.top + (rect.bottom - rect.top - size.y) / 2
	});
}

void ProjectC::Interface::Detail::WindowWin::CenterOn(IWindow& window)
{
	Size size = GetSize();
	Rect wndSize = window.GetRect();
	SetPosition(Point{
		wndSize.x + (wndSize.width - size.x) / 2,
		wndSize.y + (wndSize.height - size.y) / 2
	});
}

void ProjectC::Interface::Detail::WindowWin::SetInputCapture(bool capture)
{
	switch (capture) {
	case true:
		SetCapture(m_handle);
		break;
	case false:
		ReleaseCapture();
		break;
	}
}

ProjectC::Interface::FullscreenMode ProjectC::Interface::Detail::WindowWin::GetFullscreenMode() const
{
	throw std::exception("Fullscreen is not supported.");
}

void ProjectC::Interface::Detail::WindowWin::SetSize(const Size& size)
{
	if (!SetWindowPos(m_handle, HWND_TOPMOST, 0, 0, size.x, size.y, SWP_ASYNCWINDOWPOS | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING))
		LOG_FATAL("SetWindowPos failed: ", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::SetRect(const Rect& rect)
{
	if (!SetWindowPos(m_handle, HWND_TOPMOST, rect.x, rect.y, rect.width, rect.height, SWP_ASYNCWINDOWPOS | SWP_NOZORDER))
		LOG_FATAL("SetWindowPos failed: ", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::SetPosition(const Point& point)
{
	if (!SetWindowPos(m_handle, HWND_TOPMOST, point.x, point.y, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOZORDER | SWP_NOSIZE))
		LOG_FATAL("SetWindowPos failed: ", GetLastErrorString());
}

void ProjectC::Interface::Detail::WindowWin::SetClientSize(const Size& size)
{
	auto wndSize = GetSize();
	auto clientSize = GetClientSize();
	SetSize(Size{ wndSize.x - clientSize.x + size.x, wndSize.y - clientSize.y + size.y });
}

ProjectC::Interface::Size ProjectC::Interface::Detail::WindowWin::GetSize() const
{
	RECT rect;
	if (!GetWindowRect(m_handle, &rect))
		LOG_FATAL("GetWindowRect failed: ", GetLastErrorString());
	return Size{ rect.right - rect.left, rect.bottom - rect.top };
}

ProjectC::Interface::Size ProjectC::Interface::Detail::WindowWin::GetClientSize() const
{
	RECT rect;
	if (!GetClientRect(m_handle, &rect))
		LOG_FATAL("GetWindowRect failed: ", GetLastErrorString());
	return Size{ rect.right, rect.bottom };
}

ProjectC::Interface::Point ProjectC::Interface::Detail::WindowWin::GetPosition() const
{
	RECT rect;
	if (!GetWindowRect(m_handle, &rect))
		LOG_FATAL("GetWindowRect failed: ", GetLastErrorString());
	return Point{ rect.left, rect.top };
}

ProjectC::Interface::Rect ProjectC::Interface::Detail::WindowWin::GetRect() const
{
	RECT rect;
	if (!GetWindowRect(m_handle, &rect))
		LOG_FATAL("GetWindowRect failed: ", GetLastErrorString());
	return Rect{ rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
}

bool ProjectC::Interface::Detail::WindowWin::IsInputCaptured() const
{
	return GetCapture() == m_handle;
}

ProjectC::Interface::MonitorInfo ProjectC::Interface::Detail::WindowWin::GetMonitorInfo() const
{
	HMONITOR monitor = MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monitorInfo{};
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	if (!::GetMonitorInfoW(monitor, &monitorInfo))
		LOG_FATAL("GetMonitorInfo failed: ", GetLastErrorString());

	return MonitorInfo{
		Rect{
			monitorInfo.rcMonitor.left, 
			monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top
		},
		((monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0) ? false : true,
		UniString{monitorInfo.szDevice}
	};
}

bool ProjectC::Interface::Detail::WindowWin::IsFullscreen() const
{
	return false;
}

bool ProjectC::Interface::Detail::WindowWin::IsResizable() const
{
	return (GetWindowLong(m_handle, GWL_STYLE) & WS_SIZEBOX) != 0;
}

void ProjectC::Interface::Detail::WindowWin::SetFullscreenMode(FullscreenMode mode)
{
	throw std::exception("Fullscreen not supported.");
}

void ProjectC::Interface::Detail::WindowWin::SetResizable(bool resizable)
{
	int32_t style = GetWindowLong(m_handle, GWL_STYLE);

	switch (resizable) {
	case true:
		style |= WS_SIZEBOX;
		break;
	case false:
		style &= ~static_cast<int32_t>(WS_SIZEBOX);
		break;
	}

	SetWindowLong(m_handle, GWL_STYLE, style);
	//Has to be called for the changes to take effect.
	SetWindowPos(m_handle, NULL, NULL, NULL, NULL, NULL, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOSENDCHANGING | SWP_NOSIZE);
}

ProjectC::Interface::DisplayMode ProjectC::Interface::Detail::WindowWin::GetDisplayMode() const
{
	return DisplayMode::NORMAL;
}

void ProjectC::Interface::Detail::WindowWin::Close()
{
	Hide();
	delete this;
}

ProjectC::Interface::Detail::WindowWin::~WindowWin()
{
	m_closeListeners(*this);
	::DestroyWindow(m_handle);
}

ProjectC::Interface::Detail::BrowserWindowWin* ProjectC::Interface::Detail::WindowWin::GetBrowserWindow()
{
	return BrowserWindowWin::FromNativeHandle(m_handle);
}