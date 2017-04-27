#include "BrowserWindow.h"
#include "Detail/ProcessMessageTypes.h"
#include "../App.h"
#include "../Logging.h"
#include "BrowserHandler.h"

#include <cef/include/cef_browser.h>
#include <cef/include/cef_app.h>

ProjectC::Interface::BrowserWindow::BrowserWindow(int32_t x, int32_t y, int32_t width, int32_t height, const UniString& title) : m_statusbar(*this)
{
	CefBrowserSettings browserSettings;
	browserSettings.javascript_access_clipboard = STATE_ENABLED;
	browserSettings.javascript_close_windows = STATE_DISABLED;
	browserSettings.javascript_dom_paste = STATE_ENABLED;
	browserSettings.javascript_open_windows = STATE_DISABLED;
	browserSettings.web_security = STATE_DISABLED;
	browserSettings.universal_access_from_file_urls = STATE_ENABLED;
	browserSettings.webgl = STATE_ENABLED;
	browserSettings.background_color = 0xff323232;

	CefRefPtr<CefBrowserView> browserView = CefBrowserView::CreateBrowserView(new BrowserHandler(this), "page:root", browserSettings, nullptr, new CefBrowserViewDelegateImpl(this));
	browserView->SetSize(CefSize{ width, height });
	browserView->SetVisible(true);

	// copy to avoid risking access violation
	UniString tempTitle = title;

	m_window = std::unique_ptr<Window>{ Window::Create([x, y, browserView, tempTitle](Window& wnd) {
		wnd.GetCefWindow().SetBackgroundColor(0xff323232);
		wnd.GetCefWindow().AddChildView(browserView);
		wnd.SetPosition(CefPoint{ x, y });
		wnd.SetTitle(tempTitle);
	}) };
}

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
	
	GetBrowser().GetHost()->ShowDevTools(info, nullptr, CefBrowserSettings{}, CefPoint{});
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


// CefBrowserViewDelegateImpl
ProjectC::Interface::BrowserWindow::CefBrowserViewDelegateImpl::CefBrowserViewDelegateImpl(BrowserWindow* browserWindow) : m_browserWindow(browserWindow)
{ }

void ProjectC::Interface::BrowserWindow::CefBrowserViewDelegateImpl::OnBrowserCreated(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser)
{ 
	m_browserWindow->m_browser = browser;
}

void ProjectC::Interface::BrowserWindow::CefBrowserViewDelegateImpl::OnBrowserDestroyed(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser)
{ 
	m_browserWindow->m_browser = nullptr;
}

CefSize ProjectC::Interface::BrowserWindow::CefBrowserViewDelegateImpl::GetMinimumSize(CefRefPtr<CefView> view)
{
	return m_browserWindow->m_window->GetMinSize();
}

CefSize ProjectC::Interface::BrowserWindow::CefBrowserViewDelegateImpl::GetMaximumSize(CefRefPtr<CefView> view)
{
	return m_browserWindow->m_window->GetMaxSize();
}
