#pragma once

#include "../Platform.h"
#include <tuple>
#include <string>
#include "../UniString.h"
#include "../InvocationList.h"


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

	typedef class Point {
	public:
		int32_t x, y;

		Point(int32_t x_ = 0, int32_t y_ = 0) : x(x_), y(y_) {}
	} Size;

	class Rect {
	public:
		union {
			Point point;
			struct {
				int32_t x;
				int32_t y;
			};
		};
		union {
			Size size;
			struct {
				int32_t width;
				int32_t height;
			};
		};

		Rect(int32_t x_ = 0, int32_t y_ = 0, int32_t width_ = 0, int32_t height_ = 0) : point{x_, y_}, size{ width_, height_ } {}
		Rect(Point const& p, Size const& s) : point(p), size(s) {}
		Rect(Point p, Size s) : point(p), size(s) {}
	};

	class MonitorInfo {
	public:
		Rect rect;
		bool isPrimary;
		UniString deviceName;

		MonitorInfo() {}
		MonitorInfo(Rect rect_, bool primary, UniString name) : rect(rect_), isPrimary(primary), deviceName(name) {}
	};

	class IWindow {
	public:
		typedef InvocationList<void(IWindow&)> CloseListenerList;
		typedef InvocationList<void(IWindow&, bool shown)> ShowChangeListenerList;
		typedef InvocationList<void(IWindow&, uint32_t width, uint32_t height)> ResizedListenerList;
		typedef InvocationList<void(IWindow&, DisplayMode)> DisplayModeChangedListenerList;

		virtual void Hide() = 0;
		virtual void Show(bool show = true) = 0;

		virtual void Raise() = 0;
		virtual void Mimimize() = 0;
		virtual void Maximize() = 0;
		virtual void Restore() = 0;
		virtual void CenterOnScreen() = 0;
		virtual void CenterOnParent() = 0;
		virtual void CenterOn(IWindow& window) = 0;
		virtual void Close() = 0;

		virtual void SetTitle(UniString const& title) = 0;
		virtual void SetMaxSize(const Size& size) = 0;
		virtual void SetMinSize(const Size& size) = 0;
		virtual void SetResizable(bool resizable) = 0;
		virtual void SetFullscreenMode(FullscreenMode mode) = 0;
		virtual void SetInputCapture(bool capture) = 0;

		virtual void SetSize(const Size& size) = 0;
		virtual void SetClientSize(const Size& size) = 0;
		virtual void SetPosition(const Point& point) = 0;
		virtual void SetRect(const Rect& rect) = 0;

		virtual FullscreenMode GetFullscreenMode() const = 0;
		virtual DisplayMode GetDisplayMode() const = 0;

		virtual Size GetMinSize() const = 0;
		virtual Size GetMaxSize() const = 0;
		virtual Size GetSize() const = 0;
		virtual Size GetClientSize() const = 0;
		virtual Point GetPosition() const = 0;
		virtual Rect GetRect() const = 0;
		virtual WindowHandle GetNativeHandle() = 0;
#undef GetMonitorInfo
		virtual MonitorInfo GetMonitorInfo() const = 0;

		virtual bool IsResizable() const = 0;
		virtual bool IsFullscreen() const = 0;
		virtual bool IsInputCaptured() const = 0;

		virtual CloseListenerList& OnClose() = 0;
		virtual ShowChangeListenerList& OnShowChanged() = 0;
		virtual ResizedListenerList& OnResized() = 0;
		virtual DisplayModeChangedListenerList& OnDisplayModeChanged() = 0;
	};
}