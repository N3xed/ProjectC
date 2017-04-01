#pragma once
#include "../App.h"
#include "ConsoleWindow.h"
#include "WindowManager.h"

#include <cef/include/cef_app.h>


namespace ProjectC::Interface {
	class GUIContext : public CefApp, public CefBrowserProcessHandler {
	private:
		static GUIContext* s_instance;
	private:
		std::unique_ptr<ConsoleWindow> m_consoleWindow;
	protected:
		GUIContext(CefMainArgs& args);
	public:
		~GUIContext();

		int32_t RunMessageLoop();

		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
		//virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }

		virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) override;
		virtual void OnContextInitialized() override;

		virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) override;

	public:
		static bool Initialize(CefMainArgs& args);
		static void Shutdown();

		static void RunOnGUIThread(std::function<void()> func);

		static CefRefPtr<CefApp> GetCefAppForSubProcess();
		static GUIContext& GetInstance() {
			return *s_instance;
		}
	private:
		static void DoRegisterCustomScheme(CefRefPtr<CefSchemeRegistrar> registrar);


		IMPLEMENT_REFCOUNTING(GUIContext);
	};
}