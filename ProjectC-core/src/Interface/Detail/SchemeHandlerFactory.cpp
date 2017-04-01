#include "SchemeHandlerFactory.h"
#include "../BrowserHandler.h"
#include "ResourceHandlers.h"
#include "../../App.h"

const char ProjectC::Interface::Detail::PageSchemeHandlerFactory::PageSchemeId[] = "page";

CefRefPtr<CefResourceHandler> ProjectC::Interface::Detail::PageSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
{
	LOG_NORMAL(scheme_name, ", ", request->GetMethod(), ", ", request->GetResourceType(), ", ", request->GetURL());

	if (BrowserHandler* handler = dynamic_cast<BrowserHandler*>(browser->GetHost()->GetClient().get())) {
		BasicUniString temp;
		UniString page = StringUtils::SubStringAfterFirst(request->GetURL(), temp, ":");

		if (page == "module") {
			return new RootPageHandler<1>(std::get<0>(handler->GetBrowserWindow()->GetCurrentModule()));
		}
		else if (page == "root") {
			//return new RootPageHandler<0>();
			if (ResourceDelegate* resDelegate = ResourceManager::GetInstance().GetDelegate("root")) {
				return new ManagerResourceHandler(resDelegate);
			}
		}
	}
	return nullptr;
}

const char ProjectC::Interface::Detail::ResourceSchemeHandlerFactory::ResourceSchemeId[] = "res";

CefRefPtr<CefResourceHandler> ProjectC::Interface::Detail::ResourceSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
{
	LOG_NORMAL(scheme_name, ", ", request->GetMethod(), ", ", request->GetResourceType(), ", ", request->GetURL());

	if (BrowserHandler* handler = dynamic_cast<BrowserHandler*>(browser->GetHost()->GetClient().get())) {
		BasicUniString temp;
		UniString link = StringUtils::SubStringAfterFirst(request->GetURL(), temp, ":");
		std::vector<StringUtils::SubStringInfo<UniString::char_type>> splittedStr;
		SplitString(link, temp, ".", splittedStr);

		UniString category = splittedStr[0];
		if (splittedStr.size() >= 2) {
			if (category == "module") {
				switch (request->GetResourceType()) {
				case RT_STYLESHEET:
					return new ModuleResourceHandler<0>(std::get<0>(handler->GetBrowserWindow()->GetCurrentModule()), splittedStr[1]);
				case RT_SCRIPT:
					return new ModuleResourceHandler<1>(std::get<0>(handler->GetBrowserWindow()->GetCurrentModule()), splittedStr[1]);
				default:
					break;
				}
			}
			else if (category == "manager") {
				if (ResourceDelegate* resDelegate = ResourceManager::GetInstance().GetDelegate(splittedStr[1])) {
					return new ManagerResourceHandler(resDelegate);
				}
			}
		}
		else if (category == "root") {
			switch (request->GetResourceType()) {
			case RT_STYLESHEET:
				//return new RootResourceHandler<0>();
				if (ResourceDelegate* resDelegate = ResourceManager::GetInstance().GetDelegate("rootCSS")) {
					return new ManagerResourceHandler(resDelegate);
				}
			case RT_SCRIPT:
				return new RootResourceHandler<1>();
			default:
				break;
			}
		}
	}
	return nullptr;
}
