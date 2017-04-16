#include "BrowserHandler.h"
#include "Detail/ProcessMessageTypes.h"
#include "GUIContext.h"
#include "../ThreadManager.h"
#include "../Logging.h"

bool ProjectC::Interface::BrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	// Calls protected method here to delay the deletion.
	// If the IWindow::OnClose listener were to remove the window from the manager, memory access violation would occur.
	WindowManager::GetInstance().RemoveWindow(m_browserWindow);
	return true;
}

void ProjectC::Interface::BrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
}

ProjectC::Interface::BrowserHandler::~BrowserHandler()
{ }

ProjectC::Interface::BrowserHandler::BrowserHandler(BrowserWindow* browserWindow) : m_browserWindow(browserWindow)
{ }

void ProjectC::Interface::BrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{ }

void ProjectC::Interface::BrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	PROJC_LOG(WARN, "Failed to load: ", failedUrl, ", because: ", errorText, "(", errorCode, ")");
}

bool ProjectC::Interface::BrowserHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	PROJC_LOG(NORMAL, "ProcessMessage: ", source_process, ", ", message->GetName(), ", ", message->GetArgumentList()->GetSize());

	auto argsList = message->GetArgumentList();
	if (source_process == PID_RENDERER && message->IsValid() && message->GetName() == Detail::ProcessMessageName) {
		switch (argsList->GetInt(0)) {
		case (int32_t)Detail::BrowserProcessMessageType::EXEC_MODULE_LISTENER:
			GUIContext::RunOnGUIThread([this, message]() {
				auto argsList = message->GetArgumentList();
				auto currentModule = m_browserWindow->GetCurrentModule();
				if (std::get<2>(currentModule) != argsList->GetInt(1)) {
					return;
				}

				auto guiModule = std::get<0>(currentModule);
				if (auto listeners = guiModule->GetJSListenerList()) {
					auto iter = listeners->find(argsList->GetString(2));
					if (iter != listeners->end()) {
						try {
							iter->second(argsList->GetValue(3));
						}
						catch (const std::exception& ex) {
							PROJC_LOG(WARN, "Exception occurred: ", ex.what());
						}
					}
				}
			});
			return true;
		case (int32_t)Detail::BrowserProcessMessageType::GET_STR_RESOURCE:
			GUIContext::RunOnGUIThread([this, message]() {
				auto argsList = message->GetArgumentList();
				auto currentModule = m_browserWindow->GetCurrentModule();
				if (std::get<2>(currentModule) != argsList->GetInt(1)) {
					return;
				}

				int32_t callbackId = argsList->GetInt(2);
				UniString str = std::get<0>(currentModule)->JSGetStringResource(argsList->GetValue(3));

				auto processMsg = CefProcessMessage::Create(Detail::ProcessMessageName);
				argsList = processMsg->GetArgumentList();
				argsList->SetInt(0, static_cast<int32_t>(Detail::RenderProcessMessageType::STR_RESOURCE_RESPONSE));
				argsList->SetString(1, str);
				argsList->SetInt(2, std::get<2>(currentModule));
				argsList->SetInt(3, callbackId);

				m_browserWindow->GetBrowser().SendProcessMessage(PID_RENDERER, processMsg);
			});
			return true;
		case (int32_t)Detail::BrowserProcessMessageType::LOG:
		{
			auto argsList = message->GetArgumentList();
			std::stringstream ss;
			for (size_t i = 1; i < argsList->GetSize(); ++i) {
				StringUtils::Concatenate(ss, argsList->GetString(i));
			}
			Logging::LogMessage{ ss.str(), Logging::LogType::NORMAL };
		}
		return true;
		case (int32_t)Detail::BrowserProcessMessageType::ON_EXECUTE:
			GUIContext::RunOnGUIThread([this, message]() {
				auto argsList = message->GetArgumentList();
				auto currentModule = m_browserWindow->GetCurrentModule();
				if (std::get<2>(currentModule) != argsList->GetInt(1)) {
					return;
				}

				try {
					std::get<0>(currentModule)->JSOnExecute(argsList->GetValue(2));
				}
				catch (const std::exception& ex) {
					PROJC_LOG(WARN, "Exception occurred: ", ex.what());
				}
			});
			return true;
		case (int32_t)Detail::BrowserProcessMessageType::QUERY_RESPONSE:
			GUIContext::RunOnGUIThread([this, message]() {
				auto argsList = message->GetArgumentList();
				int32_t id = argsList->GetInt(1);
				auto& callbacks = m_browserWindow->m_queryCallbacks;
				for (auto iter = callbacks.before_begin(), eIter = callbacks.begin(); eIter != callbacks.end(); ++iter, ++eIter) {
					if (std::get<0>(*eIter) == id) {
						std::get<1>(*eIter)(argsList->GetValue(2));
						callbacks.erase_after((iter));
						return;
					}
				}
				PROJC_LOG(WARN, "Could not find a callback with id ", id);
			});
			return true;
		default:
			break;
		}
	}
	return false;
}