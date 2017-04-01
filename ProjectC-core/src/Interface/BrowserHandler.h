#pragma once
#include "WindowManager.h"
#include <cef/include/cef_client.h>
#include "../App.h"
#include <cef/include/cef_request_handler.h>


namespace ProjectC::Interface {
	class BrowserHandler : public CefClient, public CefLifeSpanHandler, public CefLoadHandler {
	protected:
		BrowserWindow* m_browserWindow;
		bool m_closing{ false };

	public:
		BrowserHandler(BrowserWindow* browserWindow);
		virtual ~BrowserHandler();

		BrowserWindow* GetBrowserWindow() { return m_browserWindow; }

		virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
		virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;

		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }

		virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
		virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override;
		
		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
		
		IMPLEMENT_REFCOUNTING(BrowserHandler);
	};
}
