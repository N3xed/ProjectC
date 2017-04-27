#include "WindowWin.h"

#include <Windows.h>


ProjectC::Interface::Detail::WindowWin::WindowWin(CreatedCallback callback) : Window(callback)
{
}

void ProjectC::Interface::Detail::WindowWin::CenterOnScreen()
{
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);

	::GetMonitorInfo(MonitorFromWindow(GetNativeHandle(), MONITOR_DEFAULTTONEAREST), &monitorInfo);
	RECT& monitorRect = monitorInfo.rcWork;

	CenterOnRect(CefRect{
		monitorRect.left,
		monitorRect.top,
		monitorRect.right - monitorRect.left,
		monitorRect.bottom - monitorRect.top
	});
}

void ProjectC::Interface::Detail::WindowWin::CenterOnParent()
{
	HWND parent = ::GetParent(GetNativeHandle());
	if (!parent)
		return;
	RECT wndRect;
	::GetWindowRect(parent, &wndRect);

	CenterOnRect(CefRect{
		wndRect.left,
		wndRect.top,
		wndRect.right - wndRect.left,
		wndRect.bottom - wndRect.top
	});
}

void ProjectC::Interface::Detail::WindowWin::CenterOn(Window& window)
{
	CenterOnRect(window.GetRect());
}

void ProjectC::Interface::Detail::WindowWin::SetFullscreenMode(FullscreenMode mode)
{
	switch (mode) {
	case FullscreenMode::ACTUAL_FULLSCREEN:
	case FullscreenMode::WINDOWED_FULLSCREEN:
		m_cefWindow->SetFullscreen(true);
	case FullscreenMode::NO_FULLSCREEN:
		m_cefWindow->SetFullscreen(false);
	}
}

void ProjectC::Interface::Detail::WindowWin::SetInputCapture(bool capture)
{
	if (capture) {
		SetCapture(GetNativeHandle());
	}
	else {
		if (IsInputCaptured())
			ReleaseCapture();
	}
}

ProjectC::Interface::FullscreenMode ProjectC::Interface::Detail::WindowWin::GetFullscreenMode() const
{
	if (m_cefWindow->IsFullscreen()) {
		return FullscreenMode::ACTUAL_FULLSCREEN;
	}
	return FullscreenMode::NO_FULLSCREEN;
}

ProjectC::Interface::MonitorInfo ProjectC::Interface::Detail::WindowWin::GetMonitorInfo() const
{
	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEX);

	::GetMonitorInfo(::MonitorFromWindow(const_cast<WindowWin*>(this)->GetNativeHandle(), MONITOR_DEFAULTTONEAREST), &monitorInfo);

	return MonitorInfo{
		CefRect{
			monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top
		},
		(monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0,
		CefString{ monitorInfo.szDevice }
	};
}

bool ProjectC::Interface::Detail::WindowWin::IsInputCaptured() const
{
	return GetCapture() == const_cast<WindowWin*>(this)->GetNativeHandle();
}
