#pragma once
#include <cef/include/cef_render_process_handler.h>
#include "../Resources/ResourceManager.h"
#include "JSApp.hpp"
#include "../../Logging.h"

namespace ProjectC::Interface::Detail {
	class RenderProcessHandler : public CefRenderProcessHandler, public CefLoadHandler {
	private:
		CefRefPtr<Detail::JSApp> m_jsApp;
	public:
		virtual void OnWebKitInitialized() override
		{
			auto& logger = Logging::Logger::GetInstance();
			logger.OnLog().Clear();
			logger.OnLog().Add([](const Logging::LogMessage& msg) {
				LOG(INFO) << msg.Message;

				const cef_string_utf16_t* strStruct = msg.Message.GetStruct();
				WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), strStruct->str, static_cast<DWORD>(strStruct->length), NULL, NULL);
			});
			
			PROJC_LOG(NORMAL, "Starting render process.");

			m_jsApp = new JSApp("", -1);

			UniString str;
			{
				std::unique_ptr<ResourceDelegate> del{ ResourceDelegate::CreateFromFile("", "D:/Projects/ProjectC/ProjectC-core/src/Interface/Resources/Scripts/app.js", true) };
				del->AsString(str);
			}
			
			if (!CefRegisterExtension("app", str, m_jsApp))
				PROJC_LOG(FATAL, "Failed to register extension.");
		}

		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
		{
			return this;
		}

		virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override
		{ }

		virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override
		{
			if (m_jsApp) {
				m_jsApp->RemoveV8Context(context);
			}
		}

		virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) override
		{
			LogFromRenderProcess(browser, StringUtils::Concatenate("[Javascript] Uncaught exception in ", exception->GetScriptResourceName(), ":", exception->GetLineNumber(), "(", exception->GetMessageW(), ") ", GetStackTrace(stackTrace)));
		}

		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override
		{
			PROJC_LOG(NORMAL, "ProcessMessage: ", source_process, ", ", message->GetName(), ", ", message->GetArgumentList()->GetSize());

			if (source_process == PID_BROWSER && m_jsApp && message->IsValid() && message->GetName() == ProcessMessageName) {
				auto argsList = message->GetArgumentList();
				switch (argsList->GetInt(0)) {
				case (int32_t)RenderProcessMessageType::EXEC_JS_LISTENER:
					m_jsApp->ExecuteRegisteredListener(argsList->GetString(1), argsList->GetValue(2));
					break;
				case (int32_t)RenderProcessMessageType::MODULE_CHANGED:
					m_jsApp->SetModule(argsList->GetString(1), argsList->GetInt(2));
					break;
				case (int32_t)RenderProcessMessageType::STR_RESOURCE_RESPONSE:
					m_jsApp->ExecuteStringResCallback(argsList->GetValue(1), argsList->GetInt(2), argsList->GetInt(3));
					break;
				case (int32_t)RenderProcessMessageType::STATUSBAR_PUSH_STATUS:

				case (int32_t)RenderProcessMessageType::STATUSBAR_ADD_CHILD:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_ALL_CHILDS:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_CHILD_BY_ID:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_CHILDS_BY_CLASS:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_CHILDS_BY_ATTR:
					return false;
				case (int32_t)RenderProcessMessageType::EXEC_JS_CODE:
				{
					PROJC_LOG(NORMAL, "EXEC_JS_CODE: ", argsList->GetString(1));
					CefRefPtr<CefValue> str = argsList->GetValue(1);
					CefPostTask(TID_RENDERER, new Detail::FunctorTask([browser, str]() {
						browser->GetMainFrame()->ExecuteJavaScript(str->GetString(), "codeFromConsole", 0);
					}));
					break;
				}
				case (int32_t)RenderProcessMessageType::WINDOW_TITLE_RESPONSE:
				{
					CefRefPtr<CefValue> str = argsList->GetValue(2);
					m_jsApp->GetCallbacks().ExecuteById(argsList->GetInt(1), [str](CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> callback) {
						callback->ExecuteFunction(nullptr, { CefV8Value::CreateString(str->GetString()) });
					});
					break;
				}
				default:
					return false;
				}
				return true;
			}

			return false;
		}

		virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override
		{
			LogFromRenderProcess(browser, StringUtils::Concatenate("Could not load URL: ", failedUrl, ", because: ", errorText, " (", errorCode, ")"));
		}


		virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override
		{
		}

	private:
		static UniString GetStackTrace(CefRefPtr<CefV8StackTrace> stackTrace) {
			std::wstringstream ss;
			for (int32_t i = 0; i < stackTrace->GetFrameCount(); ++i) {
				auto frame = stackTrace->GetFrame(i);
				ss << "\n\t(" << frame->GetScriptName().ToWString() << ")" << frame->GetFunctionName().ToWString() << ":" << frame->GetLineNumber();
			}
			return ss.str();
		}

		static void LogFromRenderProcess(CefRefPtr<CefBrowser> browser, const UniString& str) {
			PROJC_LOG(NORMAL, str);

			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto val = CefValue::Create();
			val->SetString(str);

			processMsg->GetArgumentList()->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::LOG));
			processMsg->GetArgumentList()->SetValue(1, val);
			browser->SendProcessMessage(PID_BROWSER, processMsg);
		}

		IMPLEMENT_REFCOUNTING(RenderProcessHandler);
	};
}