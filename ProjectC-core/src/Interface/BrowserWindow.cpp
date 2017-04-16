#include "BrowserWindow.h"
#include "Detail/ProcessMessageTypes.h"
#include "../App.h"
#include "../Logging.h"

#include <cef/include/cef_browser.h>
#include <cef/include/cef_client.h>
#include <cef/include/cef_app.h>

ProjectC::Interface::BrowserWindow::BrowserWindow() : m_statusbar(*this)
{ }

std::tuple<std::shared_ptr<ProjectC::Interface::IGUIModule>, ProjectC::Modules::IModule *, int32_t> ProjectC::Interface::BrowserWindow::GetCurrentModule()
{
	assert(CefCurrentlyOn(TID_UI));

	auto& module = m_modules.back();
	return std::make_tuple(module.GUIModule, module.Module, module.GUIModuleId);
}

void ProjectC::Interface::BrowserWindow::PushLayer(std::shared_ptr<IGUIModule> guiModule, Modules::IModule* module /*= nullptr*/)
{
	assert(CefCurrentlyOn(TID_UI));
	assert(guiModule.get());

	int moduleId = MakeNewId((m_modules.empty()) ? 0 : m_modules.back().GUIModuleId);
	m_modules.emplace_back(module, guiModule, moduleId);

	auto processMsg = CefProcessMessage::Create(Detail::ProcessMessageName);
	auto argsList = processMsg->GetArgumentList();
	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::MODULE_CHANGED));
	argsList->SetString(1, guiModule->GetName());
	argsList->SetInt(2, moduleId);

	GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::BrowserWindow::PopLayer()
{
	assert(CefCurrentlyOn(TID_UI));

	m_modules.pop_back();
	auto& module = m_modules.back();

	auto processMsg = CefProcessMessage::Create(Detail::ProcessMessageName);
	auto argsList = processMsg->GetArgumentList();
	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::MODULE_CHANGED));
	argsList->SetString(1, module.GUIModule->GetName());
	argsList->SetInt(2, module.GUIModuleId);

	GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

bool ProjectC::Interface::BrowserWindow::Close(bool force /*= false*/)
{
	assert(CefCurrentlyOn(TID_UI));
	if (force) {
		GetBrowser().GetHost()->CloseBrowser(true);
		return true;
	}

	return GetBrowser().GetHost()->TryCloseBrowser();
}

void ProjectC::Interface::BrowserWindow::PushStatus(const UniString& text)
{
	assert(CefCurrentlyOn(TID_UI));
	m_statusbar.PushStatus(text);
}

ProjectC::Interface::IStatusbar& ProjectC::Interface::BrowserWindow::GetStatusBar()
{
	assert(CefCurrentlyOn(TID_UI));

	return m_statusbar;
}

void ProjectC::Interface::BrowserWindow::SetNavigationVisible(bool value)
{
	assert(CefCurrentlyOn(TID_UI));

	auto processMsg = CefProcessMessage::Create(Detail::ProcessMessageName);
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::SHOW));
	argsList->SetString(1, "navigation");
	argsList->SetBool(2, value);

	GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::BrowserWindow::SetStatusbarVisible(bool value)
{
	assert(CefCurrentlyOn(TID_UI));
	m_statusbar.Show(value);
}

bool ProjectC::Interface::BrowserWindow::IsHomeScreen() const
{
	throw 0;
}

void ProjectC::Interface::BrowserWindow::ShowDevTools()
{
	CefWindowInfo info{};
	info.SetAsPopup(GetWindow().GetNativeHandle(), "DevTools");


	class DevToolsClient : public CefClient, CefLoadHandler {
	public:
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }

		virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override
		{
			PROJC_LOG(WARN, "Failed to load '", failedUrl, "' (ERROR_CODE=", static_cast<int32_t>(errorCode), ")");
		}

		virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) override
		{
			PROJC_LOG(NORMAL, "Started loading: ", frame->GetURL());
		}

		IMPLEMENT_REFCOUNTING(DevToolsClient);
	};

	GetBrowser().GetHost()->ShowDevTools(info, new DevToolsClient(), CefBrowserSettings{}, CefPoint{});
}

void ProjectC::Interface::BrowserWindow::ExecuteJSListener(const UniString& name, CefRefPtr<CefValue> arg /*= nullptr*/)
{
	assert(CefCurrentlyOn(TID_UI));

	auto processMsg = CefProcessMessage::Create(Detail::ProcessMessageName);
	auto argsList = processMsg->GetArgumentList();
	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::EXEC_JS_LISTENER));
	argsList->SetString(1, name);

	if (!arg.get()) {
		arg = CefValue::Create();
		arg->SetNull();
	}

	argsList->SetValue(2, arg);

	GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::BrowserWindow::Query(QueryType query, std::function<void(CefRefPtr<CefValue>)> callback)
{
	assert(CefCurrentlyOn(TID_UI));

	int32_t id = MakeNewId(std::get<0>(m_queryCallbacks.front()));
	m_queryCallbacks.emplace_front(std::make_tuple(id, callback));

	auto processMsg = CefProcessMessage::Create(Detail::ProcessMessageName);
	auto argsList = processMsg->GetArgumentList();
	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::QUERY));
	argsList->SetInt(1, static_cast<int32_t>(query));
	argsList->SetInt(2, id);

	GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::BrowserWindow::ExecuteJSCode(const UniString& code)
{
	assert(CefCurrentlyOn(TID_UI));

	auto processMsg = CefProcessMessage::Create(Detail::ProcessMessageName);
	auto argsList = processMsg->GetArgumentList();
	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::EXEC_JS_CODE));
	argsList->SetString(1, code);

	GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}
