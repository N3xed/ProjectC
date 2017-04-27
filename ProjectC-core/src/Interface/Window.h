#pragma once

#include <tuple>
#include <string>
#include <cef/include/views/cef_window.h>
#include <cef/include/views/cef_window_delegate.h>

#include "../UniString.h"
#include "../InvocationList.h"
#include "../Platform.h"
#include "../Logging.h"

namespace ProjectC::Interface {

	enum class FullscreenMode : uint32_t {
		WINDOWED_FULLSCREEN = 0,
		ACTUAL_FULLSCREEN = 1,
		NO_FULLSCREEN = 2
	};

	enum class DisplayMode : uint32_t {
		NORMAL = 0,
		FULLSCREEN = 1,
		MIMIMIZED = 2,
		MAXIMIZED = 3
	};

	class MonitorInfo {
	public:
		CefRect Bounds;
		bool IsPrimary;
		UniString DeviceName;

		MonitorInfo() {}
		MonitorInfo(CefRect rect_, bool primary, UniString name) : Bounds(rect_), IsPrimary(primary), DeviceName(name) {}
	};

	class Window {
	public:
		typedef InvocationList<void(Window&, bool shown)> ShowChangedListenerList;
		typedef InvocationList<void(Window&, DisplayMode)> DisplayModeChangedListenerList;

		typedef std::function<void(Window&)> CreatedCallback;

		typedef std::function<std::vector<CefDraggableRegion>(Window&)> DragRegionQueryHandler;
	protected:
		CefRefPtr<CefWindow> m_cefWindow;

		CefSize m_minSize;
		CefSize m_maxSize;
		bool m_resizable{ true };
		CreatedCallback m_createdCallback;
		DragRegionQueryHandler m_dragRegionHandler;

		ShowChangedListenerList m_showChangedListeners;
		DisplayModeChangedListenerList m_displayModeListeners;
	public:
		Window(CreatedCallback callback);

		CefWindow& GetCefWindow() { return *m_cefWindow; }

		virtual void Hide() { m_cefWindow->Hide(); }
		virtual void Show() { m_cefWindow->Show(); }

		virtual void Activate() { m_cefWindow->Activate(); }
		virtual void Deactivate() { m_cefWindow->Deactivate(); }
		virtual void Raise() { m_cefWindow->BringToTop(); }
		virtual void Mimimize() { m_cefWindow->Minimize(); }
		virtual void Maximize() { m_cefWindow->Maximize(); }
		virtual void Restore() { m_cefWindow->Restore(); }
		virtual void CenterOnScreen() = 0;
		virtual void CenterOnParent() = 0;
		virtual void CenterOnRect(const CefRect& rect);
		virtual void CenterOn(Window& window) = 0;

		virtual void SetTitle(UniString const& title) { m_cefWindow->SetTitle(title); }
		virtual void SetMaxSize(const CefSize& size) { m_maxSize = size; }
		virtual void SetMinSize(const CefSize& size) { m_minSize = size; }
		virtual void SetResizable(bool resizable) { m_resizable = resizable; }
		virtual void SetAlwaysOnTop(bool onTop) { m_cefWindow->SetAlwaysOnTop(onTop); }
		virtual void SetFullscreenMode(FullscreenMode mode) = 0;
		virtual void SetInputCapture(bool capture) = 0;
		void SetDragRegionQueryHandler(DragRegionQueryHandler handler = nullptr) { m_dragRegionHandler = handler; }

		virtual void SetSize(const CefSize& size) { m_cefWindow->SetSize(size); }
		virtual void SetPosition(const CefPoint& point) { m_cefWindow->SetPosition(point); }
		virtual void SetRect(const CefRect& rect) { m_cefWindow->SetBounds(rect); }

		virtual FullscreenMode GetFullscreenMode() const = 0;
		virtual DisplayMode GetDisplayMode() const;

		virtual UniString GetTitle() const { return m_cefWindow->GetTitle(); }
		virtual CefSize GetMinSize() const { return m_minSize; }
		virtual CefSize GetMaxSize() const { return m_maxSize; }
		virtual CefSize GetSize() const { return m_cefWindow->GetSize(); }
		virtual CefPoint GetPosition() const { return m_cefWindow->GetPosition(); }
		virtual CefRect GetRect() const { return m_cefWindow->GetBounds(); }
		virtual CefWindowHandle GetNativeHandle() { return m_cefWindow->GetWindowHandle(); }
		virtual MonitorInfo GetMonitorInfo() const = 0;

		virtual bool IsResizable() const { return m_resizable; }
		virtual bool IsFullscreen() const;
		virtual bool IsInputCaptured() const = 0;

		ShowChangedListenerList& OnShowChanged() { return m_showChangedListeners; }
		DisplayModeChangedListenerList& OnDisplayModeChanged() { return m_displayModeListeners; }

	protected:
		class CefWindowDelegateImpl : public CefWindowDelegate {
		private:
			Window* m_window;

		public:
			CefWindowDelegateImpl(Window* wnd);

			virtual bool IsFrameless(CefRefPtr<CefWindow> window) override { return true; }
			virtual bool CanResize(CefRefPtr<CefWindow> window) override { return m_window->m_resizable; }
			virtual bool CanMaximize(CefRefPtr<CefWindow> window) override { return true; }
			virtual bool CanMinimize(CefRefPtr<CefWindow> window) override { return true; }
			virtual bool CanClose(CefRefPtr<CefWindow> window) override { return true; }
			virtual CefSize GetMinimumSize(CefRefPtr<CefView> view) override { return m_window->m_minSize; }
			virtual CefSize GetMaximumSize(CefRefPtr<CefView> view) override;
			virtual void OnWindowCreated(CefRefPtr<CefWindow> window) override;

			IMPLEMENT_REFCOUNTING(CefWindowDelegateImpl);
		};

	public:
		static Window* Create(CreatedCallback callback);
	};
}