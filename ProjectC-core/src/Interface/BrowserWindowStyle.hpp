#pragma once

#include <functional>
#include <cef/include/views/cef_window.h>

#include "../UniString.h"


namespace ProjectC::Interface {

	class BrowserWindow;

	class BrowserWindowStyle {
	public:
		typedef std::function<std::vector<CefDraggableRegion>(BrowserWindow&)> DragRegionQueryHandler;
	
	private:
		UniString m_css;
		uint32_t m_windowBackgroundColor;
		DragRegionQueryHandler m_dragRegionHandler;
		CefRefPtr<CefImage> m_windowIcon;

	public:
		BrowserWindowStyle() {}

		BrowserWindowStyle& SetCSS(const UniString& css) { m_css = css; return *this; }
		BrowserWindowStyle& SetWindowBackgroundColor(uint32_t color) { m_windowBackgroundColor = color; return *this; }
		BrowserWindowStyle& SetDragRegionQueryHandler(DragRegionQueryHandler handler) { m_dragRegionHandler = handler; return *this; }
		BrowserWindowStyle& SetWindowIcon(CefRefPtr<CefImage> icon = nullptr) { m_windowIcon = icon; return *this; }

		const UniString& GetCSS() const { return m_css; }
		uint32_t GetWindowBackgroundColor() const { return m_windowBackgroundColor; }
		const DragRegionQueryHandler& GetDragRegionQueryHandler() const { return m_dragRegionHandler; }
		CefRefPtr<CefImage> GetWindowIcon() const { return m_windowIcon; }
	};
}

