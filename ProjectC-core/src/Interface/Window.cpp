#include "Window.h"
#include "../Exception.hpp"

ProjectC::Interface::Window::Window(CreatedCallback callback) : m_createdCallback(callback)
{
	CefWindow::CreateTopLevelWindow(new CefWindowDelegateImpl(this));
}

ProjectC::Interface::DisplayMode ProjectC::Interface::Window::GetDisplayMode() const
{
	FullscreenMode fullscreenMode = GetFullscreenMode();
	switch (fullscreenMode) {
	case FullscreenMode::ACTUAL_FULLSCREEN:
	case FullscreenMode::WINDOWED_FULLSCREEN:
		return DisplayMode::FULLSCREEN;
	case FullscreenMode::NO_FULLSCREEN:
		if (m_cefWindow->IsMinimized()) {
			return DisplayMode::MIMIMIZED;
		}
		else if (m_cefWindow->IsMaximized()) {
			return DisplayMode::MAXIMIZED;
		}
		break;
	}
	return DisplayMode::NORMAL;
}

bool ProjectC::Interface::Window::IsFullscreen() const
{
	switch (GetFullscreenMode()) {
	case FullscreenMode::ACTUAL_FULLSCREEN:
	case FullscreenMode::WINDOWED_FULLSCREEN:
		return true;
	default:
		return false;
	}
}

void ProjectC::Interface::Window::CenterOnRect(const CefRect& rect)
{
	CefSize wndSize = GetSize();
	SetPosition(CefPoint{
		rect.x + ((rect.width - wndSize.width) / 2),
		rect.y + ((rect.height - wndSize.height) / 2)
	});
}

#if defined(OS_WIN)
#include "Detail/Platform/WindowWin.h"
ProjectC::Interface::Window* ProjectC::Interface::Window::Create(CreatedCallback callback)
{
	return new Detail::WindowWin(callback);
}
#elif
#error No implementation for this OS.
ProjectC::Interface::Window* ProjectC::Interface::Window::Create(CreatedCallback callback, const UniString& title)
{
	throw Exception("No implementation for this OS.");
}

#endif

// CefWindowDelegateImpl
ProjectC::Interface::Window::CefWindowDelegateImpl::CefWindowDelegateImpl(Window* wnd) : m_window(wnd)
{ }

void ProjectC::Interface::Window::CefWindowDelegateImpl::OnWindowCreated(CefRefPtr<CefWindow> window)
{
	m_window->m_cefWindow = window;

	m_window->m_createdCallback(*m_window);
	m_window->m_createdCallback = nullptr;
}

CefSize ProjectC::Interface::Window::CefWindowDelegateImpl::GetMaximumSize(CefRefPtr<CefView> view)
{
	// doing it here, because there is no other place to do it in
	if (m_window->m_cefWindow && m_window->m_dragRegionHandler)
		m_window->m_cefWindow->SetDraggableRegions(m_window->m_dragRegionHandler(*m_window));

	return m_window->m_maxSize;
}
