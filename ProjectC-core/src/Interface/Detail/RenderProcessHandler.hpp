#pragma once
#include <cef/include/cef_render_process_handler.h>
#include "../Resources/ResourceManager.h"
#include "JSAppImpl.hpp"

namespace ProjectC::Interface::Detail {
	class RenderProcessHandler : public CefRenderProcessHandler, public CefLoadHandler {
	private:
		CefRefPtr<Detail::JSAppObj> m_appObj;
	public:
		virtual void OnWebKitInitialized() override
		{
			UniString str;
			{
				std::unique_ptr<ResourceDelegate> del{ ResourceDelegate::CreateFromFile("", "D:/Projects/ProjectC/ProjectC-core/src/Interface/Resources/Scripts/app.js", true) };
				del->AsString(str);
			}

			m_appObj = new Detail::JSAppObj();
			m_appObj->ResetModuleObject("", -1);

			CefRegisterExtension("v8/app", str, new Detail::JSAppObj());
		}

		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
		{
			return this;
		}

		virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override
		{
			
		}

		virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override
		{
			if (m_appObj) {
				m_appObj->RemoveListenersOfV8Context(context);
				m_appObj->GetModuleObject()->RemoveCallbacksOfV8Context(context);
			}
		}

		virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) override
		{
			LogFromRenderProcess(browser, StringUtils::Concatenate("[Javascript] Uncaught exception in ", exception->GetScriptResourceName(), ":", exception->GetLineNumber(), "(", exception->GetMessageW(), ")", GetStackTrace(stackTrace)));
		}

		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override
		{
			if (source_process == PID_BROWSER && m_appObj && message->IsValid() && message->GetArgumentList()->GetSize() > 0 && message->GetArgumentList()->GetType(0) == VTYPE_INT) {
				auto argsList = message->GetArgumentList();
				switch (argsList->GetInt(0)) {
				case (int32_t)RenderProcessMessageType::EXEC_JS_LISTENER:
					m_appObj->ExecuteJSListener(argsList->GetString(1), argsList->GetValue(2));
					break;
				case (int32_t)RenderProcessMessageType::MODULE_CHANGED:
					m_appObj->ResetModuleObject(argsList->GetString(1), argsList->GetInt(2));
					break;
				case (int32_t)RenderProcessMessageType::STR_RESOURCE_RESPONSE:
					m_appObj->GetModuleObject()->ExecuteStringResCallback(argsList->GetString(1), argsList->GetInt(2), argsList->GetInt(3));
					break;
				case (int32_t)RenderProcessMessageType::STATUSBAR_PUSH_STATUS:

				case (int32_t)RenderProcessMessageType::STATUSBAR_ADD_CHILD:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_ALL_CHILDS:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_CHILD_BY_ID:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_CHILDS_BY_CLASS:
				case (int32_t)RenderProcessMessageType::STATUSBAR_REMOVE_CHILDS_BY_ATTR:
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
			LOG(FATAL) << str << std::endl;

			auto processMsg = CefProcessMessage::Create("log");
			auto val = CefValue::Create();
			val->SetString(str);

			processMsg->GetArgumentList()->SetValue(0, val);
			browser->SendProcessMessage(PID_BROWSER, processMsg);
		}

		IMPLEMENT_REFCOUNTING(RenderProcessHandler);
	};
}