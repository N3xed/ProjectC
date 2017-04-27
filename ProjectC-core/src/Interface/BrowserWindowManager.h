#pragma once
#include <cstdint>
#include <list>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <cef/include/cef_browser.h>

#include "BrowserWindow.h"

#undef CreateWindow

namespace ProjectC::Interface {
	
	class BrowserWindowManager {
		friend class BrowserHandler;
	protected:
		std::list<std::unique_ptr<BrowserWindow>> m_windows;
		std::mutex m_lock;

		BrowserWindowManager();
 		/// <summary>
		/// Manages a window instance. Called internally by <c>WindowManager::CreateBrowserWindow</c>.
		/// </summary>
		/// <param name="window">The window.</param>
		BrowserWindow& AddWindow(BrowserWindow* window);

	public:
		virtual ~BrowserWindowManager() {}

		BrowserWindow* GetWindow(CefRefPtr<CefBrowser> browser);
		BrowserWindow* GetWindow(size_t index);

		void RemoveWindow(BrowserWindow* window);
		/// <summary>
		/// Closes all windows.
		/// Note: Only when <c>force</c> is <c>true</c> are all windows guaranteed to close.
		/// </summary>
		/// <param name="force">Used to call <c>Window::Close(force)</c>. More information in the <c>Window::Close</c> method.</param>
		/// <returns><c>true</c> if all windows were closed, <c>false</c> otherwise.</returns>
		bool CloseAllWindows(bool force);
		void CloseAllWindowsAsync(bool force, std::function<void(bool result)> callback = nullptr);

		size_t GetWindowCount() const { return m_windows.size(); }
		const std::list<std::unique_ptr<BrowserWindow>>& GetWindows() const { return m_windows; }

		static BrowserWindow& CreateWindow(const UniString& title, int32_t x, int32_t y, int32_t width, int32_t height);

		static BrowserWindowManager& GetInstance() {
			return *s_instance;
		}

		static bool Initialize();
		static void Shutdown();
	
	protected:
		static BrowserWindowManager* s_instance;
	};
}