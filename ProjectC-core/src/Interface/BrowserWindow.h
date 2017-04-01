#pragma once
#include "IWindow.h"
#include "NavigationElement.h"
#include "Detail/StatusbarImpl.h"

#include <mutex>
#include <forward_list>
#include <list>

class CefBrowser;

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
	public:

		typedef InvocationList<void(IWindow&, std::shared_ptr<IGUIModule>)> GUIModuleChangedListenerList;

		BrowserWindow();
		virtual ~BrowserWindow() {}
		
		virtual CefBrowser& GetBrowser() = 0;
		virtual IWindow& GetWindow() = 0;

		virtual std::tuple<std::shared_ptr<IGUIModule>, Modules::IModule*, int32_t> GetCurrentModule();

		void PushLayer(std::shared_ptr<IGUIModule> guiModule, Modules::IModule* module = nullptr);
		void PopLayer();
		bool Close(bool force = false);

		void PushStatus(const UniString& text);
		IStatusbar& GetStatusBar();

		void ExecuteJSListener(const UniString& name, CefRefPtr<CefValue> arg = nullptr);
		
		void Query(QueryType query, std::function<void(CefRefPtr<CefValue>)> callback);

		void ShowDevTools();

		void SetNavigationVisible(bool value);
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
	};
}