#pragma once

#include <cef/include/cef_browser.h>
#include <cef/include/cef_client.h>
#include "../../../UniString.h"
#include "../../IWindow.h"
#include <list>

namespace ProjectC::Interface::Detail {
	class WindowManagerWin;
	class BrowserWindowWin;

	class WindowWin : public IWindow {
		friend WindowManagerWin;
	protected:
		WindowHandle m_handle;
		Size m_minSize, m_maxSize;
		
		CloseListenerList m_closeListeners;
		ShowChangeListenerList m_showChangeListeners;
		ResizedListenerList m_resizedListeners;
		DisplayModeChangedListenerList m_displayModeChangedListeners;

		WindowWin(WindowHandle handle);
	public:
		~WindowWin();

		virtual void Hide() override;
		virtual void Show(bool show = true) override;

		virtual void Raise() override;
		virtual void Mimimize() override;
		virtual void Maximize() override;
		virtual void Restore() override;
		/// <summary>
		/// Destroys and deletes the window.
		/// Never call this method directly, if it was created from a <c>BrowserWindow</c>.
		/// </summary>
		virtual void Close() override;
		virtual void CenterOnScreen() override;
		virtual void CenterOnParent() override;
		virtual void CenterOn(IWindow& window) override;

		virtual void SetTitle(const UniString& title) override;
		virtual void SetInputCapture(bool capture) override;
		virtual void SetMaxSize(const Size& size) override { m_maxSize = size; }
		virtual void SetMinSize(const Size& size) override { m_minSize = size; }
		virtual void SetResizable(bool resizable) override;
		virtual void SetFullscreenMode(FullscreenMode mode) override;
		virtual void SetSize(const Size& size) override;
		virtual void SetClientSize(const Size& size) override;
		virtual void SetPosition(const Point& point) override;
		virtual void SetRect(const Rect& rect) override;

		virtual FullscreenMode GetFullscreenMode() const override;
		virtual DisplayMode GetDisplayMode() const override;
		virtual Size GetMinSize() const override { return m_minSize; }
		virtual Size GetMaxSize() const override { return m_maxSize; }
		virtual Size GetSize() const override;
		virtual Size GetClientSize() const override;
		virtual Point GetPosition() const override;
		virtual Rect GetRect() const override;
#undef GetMonitorInfo
		virtual MonitorInfo GetMonitorInfo() const override;

		virtual bool IsResizable() const override;
		virtual bool IsFullscreen() const override;
		virtual bool IsInputCaptured() const override;

		virtual WindowHandle GetNativeHandle() override { return m_handle; }

		virtual CloseListenerList& OnClose() override { return m_closeListeners; }
		virtual ShowChangeListenerList& OnShowChanged() override { return m_showChangeListeners; }
		virtual ResizedListenerList& OnResized() override { return m_resizedListeners; }
		virtual DisplayModeChangedListenerList& OnDisplayModeChanged() override { return m_displayModeChangedListeners; }

		BrowserWindowWin* GetBrowserWindow();

		static UniString GetLastErrorString();
		static WindowWin* Create(const UniString& title, int32_t x, int32_t y, int32_t width, int32_t height, HINSTANCE hInstance, WindowHandle parent = nullptr, bool defaultPos = false);

	};
}
