#pragma once
#include <cstdint>
#include <list>
#include <unordered_set>
#include "BrowserWindow.h"
#include <mutex>
#include <memory>
#include <cef/include/cef_browser.h>

namespace ProjectC::Interface {
	
	class WindowManager {
		friend class BrowserHandler;
	protected:
		std::list<std::unique_ptr<BrowserWindow>> m_windows;
		std::mutex m_lock;

		WindowManager();
 		/// <summary>
		/// Manages a window instance. Called internally by <c>WindowManager::CreateBrowserWindow</c>.
		/// </summary>
		/// <param name="window">The window.</param>
		/// <returns>Its unique id.</returns>
		void AddWindow(BrowserWindow* window);
		/// <summary>
		/// Initializes the framework for window management.
		/// Gets called by WindowManager::Initialize for plattform specific intialization.
		/// </summary>
		/// <returns><c>true</c> for success, <c>false</c> otherwise.</returns>
		virtual bool PlatformInitialize() = 0;
		void RemoveWindow(BrowserWindow* window);

		virtual BrowserWindow* DoGetWindow(WindowHandle handle);
		virtual BrowserWindow* DoGetWindow(IWindow& window);
		virtual BrowserWindow* DoGetWindow(CefRefPtr<CefBrowser> browser);
	public:
		virtual ~WindowManager() {}

		/// <summary>
		/// Gets a window, which has been added using <c>AddWindow</c>.
		/// </summary>
		/// <param name="handle">The handle.</param>
		/// <returns>The Window on success, <c>nullptr</c> on failure.</returns>
		BrowserWindow* GetWindow(WindowHandle handle);
		BrowserWindow* GetWindow(IWindow& window);
		BrowserWindow* GetWindow(CefRefPtr<CefBrowser> browser);
		BrowserWindow* GetWindow(size_t index);

		/// <summary>
		/// Closes all windows.
		/// Note: Only when <c>force</c> is <c>true</c> are all windows guaranteed to close.
		/// </summary>
		/// <param name="force">Used to call <c>Window::Close(force)</c>. More information in the <c>Window::Close</c> method.</param>
		/// <returns><c>true</c> if all windows were closed, <c>false</c> otherwise.</returns>
		bool CloseAllWindows(bool force);
		void CloseAllWindowsAsync(bool force, std::function<void(bool result)> callback = nullptr);

		size_t GetWindowCount() { return m_windows.size(); }
		std::list<std::unique_ptr<BrowserWindow>>& GetWindows() { return m_windows; }

		static WindowManager& GetInstance() {
			return *s_instance;
		}

		static void RegisterSchemeHandlerFactories();

		static BrowserWindow& CreateBrowserWindow(const UniString& title, int32_t x, int32_t y, int32_t width = -1, int32_t height = -1, IWindow* parent = nullptr);
		static BrowserWindow& CreateBrowserWindow(const UniString& title, int32_t width = -1, int32_t height = -1, IWindow* parent = nullptr);

		static bool Initialize();
		static void Shutdown();
	
	protected:
		static WindowManager* s_instance;
	};
}