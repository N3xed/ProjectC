#include "StatusbarImpl.h"
#include "../BrowserWindow.h"


void ProjectC::Interface::Detail::StatusbarImpl::RemoveChilds()
{
	auto processMsg = CefProcessMessage::Create("");
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::STATUSBAR_REMOVE_ALL_CHILDS));

	m_browserWindow.GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::Detail::StatusbarImpl::RemoveChilds(const UniString& attributeName, const UniString& value)
{
	auto processMsg = CefProcessMessage::Create("");
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::STATUSBAR_REMOVE_CHILDS_BY_ATTR));
	argsList->SetString(1, attributeName);
	argsList->SetString(2, value);

	m_browserWindow.GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::Detail::StatusbarImpl::RemoveChilds(const UniString& className)
{
	auto processMsg = CefProcessMessage::Create("");
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::STATUSBAR_REMOVE_CHILDS_BY_CLASS));
	argsList->SetString(1, className);

	m_browserWindow.GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::Detail::StatusbarImpl::RemoveChild(const UniString& id)
{
	auto processMsg = CefProcessMessage::Create("");
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::STATUSBAR_REMOVE_CHILD_BY_ID));
	argsList->SetString(1, id);

	m_browserWindow.GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::Detail::StatusbarImpl::AddChild(const UniString& htmlCode)
{
	auto processMsg = CefProcessMessage::Create("");
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::STATUSBAR_ADD_CHILD));
	argsList->SetString(1, htmlCode);

	m_browserWindow.GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::Detail::StatusbarImpl::PushStatus(const UniString& str)
{
	auto processMsg = CefProcessMessage::Create("");
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::STATUSBAR_PUSH_STATUS));
	argsList->SetString(1, str);

	m_browserWindow.GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}

void ProjectC::Interface::Detail::StatusbarImpl::Show(bool show /*= true*/)
{
	auto processMsg = CefProcessMessage::Create("");
	auto argsList = processMsg->GetArgumentList();

	argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::SHOW));
	argsList->SetString(1, "statusbar");
	argsList->SetBool(2, show);

	m_browserWindow.GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
}


ProjectC::Interface::Detail::StatusbarImpl::StatusbarImpl(BrowserWindow& browserWindow) : m_browserWindow(browserWindow)
{ }
