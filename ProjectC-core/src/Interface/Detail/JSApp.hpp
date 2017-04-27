#pragma once
#include <mutex>
#include <list>
#include <algorithm>

#include "V8HandlerList.hpp"
#include "../../UniString.h"
#include "../../Utils/String.h"
#include "../../App.h"
#include "ProcessMessageTypes.h"
#include "FunctorTask.hpp"
#include "../../Logging.h"
#include "V8Utils.hpp"

namespace ProjectC::Interface::Detail {

	class JSApp : public V8HandlerList<JSApp> {
	private:
		class ModuleInfo {
		public:
			int32_t Id;
			CefString Name;
			V8CallbackList StringResCallbacks;

			ModuleInfo(CefString name, int32_t id) : Name(name), Id(id) {}
		};

		ModuleInfo m_module;
		std::mutex m_moduleLock;

		std::list<std::pair<CefRefPtr<CefV8Context>, std::unordered_map<UniString, CefRefPtr<CefV8Value>>>> m_listeners;
		std::mutex m_listenersLock;
		V8CallbackList m_callbacks;
	public:
		JSApp(CefString moduleName, int32_t moduleId) :
			V8HandlerList({
				{ "nLog", &JSApp::nLog },
				{ "nRegisterListener", &JSApp::nRegisterListener },
				{ "nUnregisterListener", &JSApp::nUnregisterListener },
				{ "nGetModuleName", &JSApp::nGetModuleName },
				{ "nGetModuleId", &JSApp::nGetModuleId },
				{ "nExecModuleListener", &JSApp::nExecModuleListener },
				{ "nGetStrRes", &JSApp::nGetStrRes },
				{ "nFireOnExec", &JSApp::nFireOnExec },
				{ "nGetWindowTitle", &JSApp::nGetWindowTitle },
				{ "nSetWindowTitle", &JSApp::nSetWindowTitle },
				{ "nShowWindow", &JSApp::nShowWindow },
				{ "nCloseWindow", &JSApp::nCloseWindow },
				{ "nOpenWindow", &JSApp::nOpenWindow },
			}),
			m_module(moduleName, moduleId)
		{ }

		virtual ~JSApp() {}

		void SetModule(CefString name, int32_t id) {
			std::lock_guard<std::mutex> guard{ m_moduleLock };
			m_module.Name = name;
			m_module.Id = id;
			m_module.StringResCallbacks.Clear();
		}

		void ExecuteStringResCallback(CefRefPtr<CefValue> res, int32_t moduleId, int32_t callbackId) {
			std::lock_guard<std::mutex> guard{ m_moduleLock };
			if (m_module.Id != moduleId)
				return;

			m_module.StringResCallbacks.ExecuteById(callbackId, [res](CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> callback) {
				callback->ExecuteFunction(nullptr, { CefV8Value::CreateString(res->GetString()) });
			});
		}

		void RemoveV8Context(CefRefPtr<CefV8Context> context) {
			{
				std::lock_guard<std::mutex> guard{ m_moduleLock };
				m_module.StringResCallbacks.RemoveByContext(context);
			}
			{
				std::lock_guard<std::mutex> guard{ m_listenersLock };
				m_listeners.remove_if([&context](auto& e) {
					return context->IsSame(e.first);
				});
			}
		}

		void ExecuteRegisteredListener(const CefString& key, CefRefPtr<CefValue> val) {
			std::lock_guard<std::mutex> guard{ m_listenersLock };

			for (auto& e : m_listeners) {
				auto iter = e.second.find(key);
				if (iter != e.second.end()) {

					CefRefPtr<CefV8Context> context = e.first;
					CefRefPtr<CefV8Value> function = iter->second;
					e.second.erase(iter);
					context->GetTaskRunner()->PostTask(new V8ContextFunctorTask([val, function, this]() {
						CefRefPtr<CefV8Value> v8Value;
						ValueToV8Value(v8Value, val);

						function->ExecuteFunction(nullptr, { v8Value });
					}, context));
					return;
				}
			}
		}

		V8CallbackList& GetCallbacks() { return m_callbacks; }

	private:
		static int32_t MakeId() {
			static uint32_t id{ 0 };
			return static_cast<int32_t>(++id);
		}

		// sends process message with arguments: BrowserProcessMessageType::LOG, Message(CefString)
		static void nLog(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() == 0)
				return;
			PROJC_LOG(NORMAL, args.size());
			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto valueList = processMsg->GetArgumentList();
			valueList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::LOG));

			BasicUniStringStream ss;
			for (size_t i = 0; i < args.size(); ++i) {
				if (i != 0)
					ss << ", ";
				V8ValueToString(ss, args[i]);
			}
			valueList->SetString(1, ss.str());

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_RENDERER, processMsg);
		}

		static void nRegisterListener(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
				exception = "Invalid arguments: The first argument isn't a string or the second not a function.";
				return;
			}
			std::lock_guard<std::mutex> guard{ sender.m_listenersLock };
			auto iter = std::find_if(sender.m_listeners.begin(), sender.m_listeners.end(), [](auto& e) {
				return CefV8Context::GetCurrentContext()->IsSame(e.first);
			});
			if (iter == sender.m_listeners.end()) {
				sender.m_listeners.emplace_back(std::make_pair(
					CefV8Context::GetCurrentContext(),
					std::unordered_map<UniString, CefRefPtr<CefV8Value>>{
						{
							{
								args[0]->GetStringValue(), args[1]
							}
						}
				}));
			}
			else {
				std::unordered_map<UniString, CefRefPtr<CefV8Value>>& listeners = iter->second;
				if (!listeners.emplace(args[0]->GetStringValue(), args[1]).second) {
					exception = "Key already exists.";
				}
			}
		}

		static void nUnregisterListener(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 1 || !args[0]->IsString()) {
				exception = "Invalid arguments: The first argument isn't a string.";
				return;
			}
			std::lock_guard<std::mutex> guard{ sender.m_listenersLock };
			auto iter = std::find_if(sender.m_listeners.begin(), sender.m_listeners.end(), [](auto& e) {
				return CefV8Context::GetCurrentContext()->IsSame(e.first);
			});
			if (iter == sender.m_listeners.end()) {
				retval = CefV8Value::CreateBool(false);
				return;
			}
			auto iter1 = iter->second.find(args[0]->GetStringValue());
			if (iter1 == iter->second.end()) {
				retval = CefV8Value::CreateBool(false);
				return;
			}
			iter->second.erase(iter1);
			retval = CefV8Value::CreateBool(true);
		}

		static void nGetModuleName(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			std::lock_guard<std::mutex> guard{ sender.m_moduleLock };
			retval = CefV8Value::CreateString(sender.m_module.Name);
		}

		static void nGetModuleId(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			std::lock_guard<std::mutex> guard{ sender.m_moduleLock };
			retval = CefV8Value::CreateInt(sender.m_module.Id);
		}

		// sends process message with arguments: BrowserProcessMessageType::EXEC_MODULE_LISTENER, ModuleId(int32_t), ListenerKey(CefString), ListenerArg(obj)
		static void nExecModuleListener(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 2 || !args[0]->IsString()) {
				exception = "Invalid arguments: First argument isn't a string or not enough arguments.";
				return;
			}

			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto argsList = processMsg->GetArgumentList();
			argsList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::EXEC_MODULE_LISTENER));

			std::lock_guard<std::mutex> guard{ sender.m_moduleLock }; // locking for thread safety
			argsList->SetInt(1, sender.m_module.Id);
			argsList->SetString(2, args[0]->GetStringValue());

			CefRefPtr<CefValue> arg{};
			V8ValueToValue(arg, args[1]);
			argsList->SetValue(3, arg);

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
		}

		// sends process message with arguments: BrowserProcessMessageType::GET_STR_RESOURCE, ModuleId(int32_t), CallbackId(int32_t), Argument(obj)
		static void nGetStrRes(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 2 || args[1]->IsFunction()) {
				exception = "Invalid arguments: Not enough arguments or second argument is not a function.";
				return;
			}
			std::lock_guard<std::mutex> moduleGuard{ sender.m_moduleLock };

			int32_t callbackId = MakeId();
			sender.m_module.StringResCallbacks.AddCallback(callbackId, CefV8Context::GetCurrentContext(), args[1]);

			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto argsList = processMsg->GetArgumentList();
			argsList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::GET_STR_RESOURCE));
			argsList->SetInt(1, sender.m_module.Id);
			argsList->SetInt(2, callbackId);

			CefRefPtr<CefValue> arg;
			V8ValueToValue(arg, args[0]);
			argsList->SetValue(3, arg);

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
		}

		// sends process message with arguments: BrowserProcessMessageType::ON_EXECUTE, ModuleId(int32_t), Argument(obj)
		static void nFireOnExec(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 1) {
				exception = "Invalid argument: Not enough arguments.";
				return;
			}

			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto argsList = processMsg->GetArgumentList();
			argsList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::GET_STR_RESOURCE));

			std::lock_guard<std::mutex> moduleGuard{ sender.m_moduleLock }; // locking for thread safety
			argsList->SetInt(1, sender.m_module.Id);

			CefRefPtr<CefValue> arg;
			V8ValueToValue(arg, args[0]);
			argsList->SetValue(2, arg);

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
		}

		static void nGetWindowTitle(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 1 || !args[0]->IsFunction()) {
				exception = "Invalid argument: First argument is not a function.";
				return;
			}

			int32_t callbackId = MakeId();
			sender.m_callbacks.AddCallback(callbackId, CefV8Context::GetCurrentContext(), args[0]);

			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto argsList = processMsg->GetArgumentList();
			argsList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::GET_WINDOW_TITLE));
			argsList->SetInt(1, callbackId);

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
		}

		static void nSetWindowTitle(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 1 || !args[0]->IsString()) {
				exception = "Invalid argument: First argument is not a string.";
				return;
			}

			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto argsList = processMsg->GetArgumentList();
			argsList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::SET_WINDOW_TITLE));
			argsList->SetString(1, args[0]->GetStringValue());

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
		}

		static void nShowWindow(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			if (args.size() < 1 || !args[0]->IsBool()) {
				exception = "Invalid argument: First argument is not a boolean.";
				return;
			}

			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto argsList = processMsg->GetArgumentList();
			argsList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::SHOW_WINDOW));
			argsList->SetBool(1, args[0]->GetBoolValue());

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
		}

		static void nCloseWindow(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			auto processMsg = CefProcessMessage::Create(ProcessMessageName);
			auto argsList = processMsg->GetArgumentList();
			argsList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::CLOSE_WINDOW));

			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
		}

		static void nOpenWindow(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, JSApp& sender) {
			exception = "Not implemented.";
		}
	};
}