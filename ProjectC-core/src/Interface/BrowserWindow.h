#pragma once
#include <mutex>
#include <forward_list>
#include <list>
#include <cef/include/views/cef_browser_view.h>
#include <cef/include/views/cef_browser_view_delegate.h>

#include "NavigationElement.h"
#include "Detail/StatusbarImpl.h"
#include "Window.h"
#include "BrowserWindowStyle.hpp"

namespace ProjectC::Modules {
	class IModule;
}

namespace ProjectC::Interface {
	class IGUIModule;
	class IStatusbar;
	class DisplayerControl;
	
	class BrowserWindow {
		friend class BrowserHandler;
	public:
		enum class QueryType : int32_t {
			STATUSBAR_VISIBLE = 0,
			NAVIGATION_VISIBLE = 2,
			ID_VISIBLE = 3
		};

	protected:
		std::list<NavigationElement> m_modules;
		std::forward_list<std::tuple<int32_t, std::function<void(CefRefPtr<CefValue>)>>> m_queryCallbacks;
		Detail::StatusbarImpl m_statusbar;
		BrowserWindowStyle m_windowStyle;

		std::unique_ptr<Window> m_window;
		CefRefPtr<CefBrowser> m_browser;
	public:
		typedef InvocationList<void(BrowserWindow&, std::shared_ptr<IGUIModule>)> GUIModuleChangedListenerList;

		BrowserWindow(int32_t x, int32_t y, int32_t width, int32_t height, const UniString& title = "");
		virtual ~BrowserWindow() {}
		
		virtual CefBrowser& GetBrowser() { return *m_browser; }
		virtual Window& GetWindow() { return *m_window; }

		std::tuple<std::shared_ptr<IGUIModule>, Modules::IModule*, int32_t> GetCurrentModule();

		void PushLayer(std::shared_ptr<IGUIModule> guiModule, Modules::IModule* module = nullptr);
		void PopLayer();
		bool Close(bool force = false);

		void PushStatus(const UniString& text);
		IStatusbar& GetStatusBar();

		void ExecuteJSListener(const UniString& name, CefRefPtr<CefValue> arg = nullptr);
		void ExecuteJSCode(const UniString& code);
		
		void Query(QueryType query, std::function<void(CefRefPtr<CefValue>)> callback);

		void ShowDevTools();

		void SetStatusbarVisible(bool value);
		bool IsHomeScreen() const;

	protected:
		int32_t MakeNewId(int32_t old) {
			switch (old) {
			case INT32_MAX:
				return 0;
			default:
				return old + 1;
			}
		}

	protected:
		class CefBrowserViewDelegateImpl : public CefBrowserViewDelegate {
		private:
			BrowserWindow* m_browserWindow;

		public:
			CefBrowserViewDelegateImpl(BrowserWindow* browserWindow);

			virtual void OnBrowserCreated(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser) override;
			virtual void OnBrowserDestroyed(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser) override;
			virtual CefSize GetMinimumSize(CefRefPtr<CefView> view) override;
			virtual CefSize GetMaximumSize(CefRefPtr<CefView> view) override;

			IMPLEMENT_REFCOUNTING(CefBrowserViewDelegateImpl);
		};
	};
}