#include "SchemeHandlerFactory.h"
#include "../BrowserHandler.h"
#include "ResourceHandlers.h"
#include "../../Logging.h"

const char ProjectC::Interface::Detail::PageSchemeHandlerFactory::PageSchemeId[] = "page";

CefRefPtr<CefResourceHandler> ProjectC::Interface::Detail::PageSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
{
	PROJC_LOG(NORMAL, scheme_name, ", ", request->GetMethod(), ", ", request->GetResourceType(), ", ", request->GetURL());

	if (BrowserHandler* handler = dynamic_cast<BrowserHandler*>(browser->GetHost()->GetClient().get())) {
		BasicUniString temp;
		UniString page = StringUtils::SubStringAfterFirst(request->GetURL(), temp, ":");

		if (page == "module") {
			return new ModulePageHandler(std::get<0>(handler->GetBrowserWindow()->GetCurrentModule()));
		}
		else if (page == "root") {
			//return new RootPageHandler();
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
	PROJC_LOG(NORMAL, scheme_name, ", ", request->GetMethod(), ", ", request->GetResourceType(), ", ", request->GetURL());

	if (BrowserHandler* handler = dynamic_cast<BrowserHandler*>(browser->GetHost()->GetClient().get())) {
		BasicUniString temp;
		UniString link = StringUtils::SubStringAfterFirst(request->GetURL(), temp, ":");
		std::vector<StringUtils::SubStringInfo<UniString::char_type>> splittedStr;
		SplitString(link, temp, "/", splittedStr);

		if (splittedStr.size() < 2)
			return nullptr;

		UniString category{ splittedStr[0].String, splittedStr[0].Length, false };
		if (category == "module") {
			if (splittedStr.size() < 3) {
				PROJC_LOG(WARN, "URL '", request->GetURL(), "' could not be evaluated to a resource. (key missing)");
			}
			UniString type{ splittedStr[1].String, splittedStr[1].Length, false };
			UniString key{ splittedStr[2].String, link.length() - (splittedStr[0].Length + splittedStr[1].Length), false };

			if (type == "css") {
				return new ModuleResourceHandler(std::get<0>(handler->GetBrowserWindow()->GetCurrentModule()), key, ResourceType::CSS);
			}
			else if (type == "js") {
				return new ModuleResourceHandler(std::get<0>(handler->GetBrowserWindow()->GetCurrentModule()), key, ResourceType::JS);
			}
			else if (type == "res") {
				return new ModuleResourceHandler(std::get<0>(handler->GetBrowserWindow()->GetCurrentModule()), key, ResourceType::UNKNOWN);
			}
		}
		else if (category == "manager") {
			if (splittedStr.size() < 2) {
				PROJC_LOG(WARN, "URL '", request->GetURL(), "' could not be evaluated to a resource. (key missing)");
			}
			UniString key{ splittedStr[1].String, link.length() - splittedStr[0].Length, false };

			if (ResourceDelegate* resDelegate = ResourceManager::GetInstance().GetDelegate(splittedStr[1].String)) {
				return new ManagerResourceHandler(resDelegate);
			}
			else {
				PROJC_LOG(WARN, "No resource delegate with key '", key, "' found.");
			}
		}
		else if (category == "root") {
			UniString type{ splittedStr[1].String, splittedStr[1].Length, false };

			if (type == "css") {
				//return new RootResourceHandler<ResourceType::CSS>();
				if (ResourceDelegate* resDelegate = ResourceManager::GetInstance().GetDelegate("rootCSS")) {
					return new ManagerResourceHandler(resDelegate);
				}
			}
			else if (type == "js") {
				//return new RootResourceHandler<ResourceType::JS>();
				if (ResourceDelegate* resDelegate = ResourceManager::GetInstance().GetDelegate("rootJS")) {
					return new ManagerResourceHandler(resDelegate);
				}
			}
		}
	}
	return nullptr;
}
