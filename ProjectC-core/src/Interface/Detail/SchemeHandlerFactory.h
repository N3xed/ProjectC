#pragma once
#include <cef/include/cef_scheme.h>

namespace ProjectC::Interface::Detail {
	class PageSchemeHandlerFactory : public CefSchemeHandlerFactory {
	public:
		static const char PageSchemeId[];

		virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) override;

		IMPLEMENT_REFCOUNTING(PageSchemeHandlerFactory);
	};

	class ResourceSchemeHandlerFactory : public CefSchemeHandlerFactory {
	public:
		static const char ResourceSchemeId[];

		virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) override;

		IMPLEMENT_REFCOUNTING(ResourceSchemeHandlerFactory);
	};
}

