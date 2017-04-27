#include "WindowManagerWin.h"
#include "../../../Logging.h"

#include <windows.h>
#include <Windowsx.h>


const wchar_t ProjectC::Interface::Detail::WindowManagerWin::WINDOW_CLASS_NAME[] = L"ProjectCWindow";

ATOM ProjectC::Interface::Detail::WindowManagerWin::WINDOW_CLASS;

ProjectC::Interface::Detail::WindowManagerWin::~WindowManagerWin()
{
	CloseAllWindows(true);
}

bool ProjectC::Interface::Detail::WindowManagerWin::PlatformInitialize()
{
	WNDCLASS windowClass;
	windowClass.lpfnWndProc = &WindowProcedure;
	windowClass.style = CS_PARENTDC;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.lpszClassName = WINDOW_CLASS_NAME;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;

	WINDOW_CLASS = RegisterClass(&windowClass);
	if (WINDOW_CLASS == NULL) {
		PROJC_LOG(FATAL, "RegisterClass failed. (ERRCODE: ", ::GetLastError(), ")");
		return false;
	}
	return true;
}

LRESULT CALLBACK ProjectC::Interface::Detail::WindowManagerWin::WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CLOSE:
		BrowserWindowWin::FromNativeHandle(hwnd)->Close(false);
		return 0;
		break;
	/*case WM_NCACTIVATE:
		return TRUE;*/
	case WM_NCHITTEST:
		PROJC_LOG(NORMAL, "hit-test");
		break;
	case WM_MOUSEMOVE:
	{
		PROJC_LOG(NORMAL, GET_X_LPARAM(lParam), ",", GET_Y_LPARAM(lParam));

		break;
	}
	case WM_SIZE:
	{
		int16_t width = static_cast<int16_t>(lParam);
		int16_t height = static_cast<int16_t>(lParam >> 16);
		BrowserWindowWin* browserWindow = BrowserWindowWin::FromNativeHandle(hwnd);
		if (!browserWindow)
			return 0;
		SetWindowPos(browserWindow->GetBrowser().GetHost()->GetWindowHandle(), HWND_TOPMOST, 0, 0, width, height, SWP_ASYNCWINDOWPOS | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
		browserWindow->GetWindow().OnResized()(browserWindow->GetWindow(), static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		return 0;
	}
	case WM_GETMINMAXINFO:
	{
		BrowserWindowWin* wnd = BrowserWindowWin::FromNativeHandle(hwnd);
		if (!wnd)
			return 0;
		MINMAXINFO& mmInfo = *reinterpret_cast<MINMAXINFO*>(lParam);
		Size minSize = wnd->GetWindow().GetMinSize();
		Size maxSize = wnd->GetWindow().GetMaxSize();
		if (minSize.x > 0 && minSize.y > 0) {
			mmInfo.ptMinTrackSize.x = minSize.x;
			mmInfo.ptMinTrackSize.y = minSize.y;
		}
		else if (maxSize.x == 0 && maxSize.y == 0) {
		}
		if (maxSize.x > 0 && maxSize.y > 0) {
			mmInfo.ptMaxTrackSize.x = maxSize.x;
			mmInfo.ptMaxTrackSize.y = maxSize.y;
		}
		return 0;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::shared_ptr<ProjectC::Interface::BrowserWindow> ProjectC::Interface::Detail::WindowManagerWin::DoGetWindow(WindowHandle handle)
{
	return std::shared_ptr<BrowserWindow>{BrowserWindowWin::FromNativeHandle(handle)};
}
