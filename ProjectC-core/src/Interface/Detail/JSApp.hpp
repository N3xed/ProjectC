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

namespace ProjectC::Interface::Detail {

	static void V8ValueToString(BasicUniStringStream& strStream, CefRefPtr<CefV8Value> value) {
		if (value->IsUndefined()) {
			strStream << "undefined";
		}
		else if (value->IsNull()) {
			strStream << "null";
		}
		else if (value->IsArray()) {
			strStream << "{ ";

			for (int32_t i = 0; i < value->GetArrayLength(); ++i) {
				if (i != 0) {
					strStream << ", ";
				}
				strStream << "'" << i << "' : '";
				V8ValueToString(strStream, value->GetValue(i));
				strStream << "'";
			}
			strStream << " }";
		}
		else if (value->IsBool()) {
			if (value->GetBoolValue()) {
				strStream << "true";
			}
			else {
				strStream << "false";
			}
		}
		else if (value->IsDate()) {
			CefTime time = value->GetDateValue();
			strStream << StringUtils::Concatenate<CefString::char_type>(time.hour, ":", time.minute, ":", time.second, "/", time.day_of_month, ".", time.month, ".", time.year);
		}
		else if (value->IsDouble()) {
			strStream << std::to_string(value->GetDoubleValue());
		}
		else if (value->IsFunction()) {
			strStream << StringUtils::Concatenate<CefString::char_type>("function ", value->GetFunctionName(), "()");
		}
		else if (value->IsInt()) {
			strStream << std::to_string(value->GetIntValue());
		}
		else if (value->IsObject()) {
			strStream << "{ ";

			std::vector<UniString> keys;
			value->GetKeys(keys);
			for (size_t i = 0; i < keys.size(); ++i) {
				if (i != 0)
					strStream << ", ";
				strStream << "'" << keys[i] << "' : '";
				V8ValueToString(strStream, value->GetValue(keys[i]));
				strStream << "'";
			}
			strStream << " }";
		}
		else if (value->IsString()) {
			strStream << value->GetStringValue();
		}
		else if (value->IsUInt()) {
			strStream << std::to_string(value->GetUIntValue());
		}
	}

	static void V8ValueToValue(CefRefPtr<CefValue>& result, CefRefPtr<CefV8Value> value) {
		result = CefValue::Create();
		if (value->IsUndefined()) {
			result->SetNull();
		}
		else if (value->IsNull()) {
			result->SetNull();
		}
		else if (value->IsArray()) {
			CefRefPtr<CefListValue> list = CefListValue::Create();
			list->SetSize(value->GetArrayLength());
			for (int32_t i = 0; i < value->GetArrayLength(); ++i) {
				CefRefPtr<CefValue> resultVal;
				V8ValueToValue(resultVal, value->GetValue(i));
				list->SetValue(i, resultVal);
			}
			result->SetList(list);
		}
		else if (value->IsBool()) {
			result->SetBool(value->GetBoolValue());
		}
		else if (value->IsDate()) {
			CefTime time = value->GetDateValue();
			CefString str = StringUtils::Concatenate<CefString::char_type>(time.hour, ":", time.minute, ":", time.second, "/", time.day_of_month, ".", time.month, ".", time.year);

			result->SetString(str);
		}
		else if (value->IsDouble()) {
			result->SetDouble(value->GetDoubleValue());
		}
		else if (value->IsFunction()) {
		}
		else if (value->IsInt()) {
			result->SetInt(value->GetIntValue());
		}
		else if (value->IsObject()) {
			auto dictionary = CefDictionaryValue::Create();

			std::vector<CefString> keys;
			value->GetKeys(keys);
			for (auto& e : keys) {
				CefRefPtr<CefValue> resultVal;
				V8ValueToValue(resultVal, value->GetValue(e));
				dictionary->SetValue(e, resultVal);
			}
		}
		else if (value->IsString()) {
			result->SetString(value->GetStringValue());
		}
		else if (value->IsUInt()) {
			result->SetInt(static_cast<int>(value->GetUIntValue()));
		}
	}

	static void ValueToV8Value(CefRefPtr<CefV8Value>& result, CefRefPtr<CefValue> value) {
		switch (value->GetType()) {
		case VTYPE_NULL:
			result = CefV8Value::CreateNull();
			break;
		case VTYPE_BOOL:
			result = CefV8Value::CreateBool(value->GetBool());
			break;
		case VTYPE_INVALID:
			result = CefV8Value::CreateNull();
			break;
		case VTYPE_INT:
			result = CefV8Value::CreateInt(value->GetInt());
			break;
		case VTYPE_DOUBLE:
			result = CefV8Value::CreateDouble(value->GetDouble());
			break;
		case VTYPE_STRING:
			result = CefV8Value::CreateString(value->GetString());
			break;
		case VTYPE_BINARY:
			PROJC_LOG(WARN, "Converting binary data as CefValue to CefV8Value is forbidden.");
			break;
		case VTYPE_DICTIONARY:
			result = CefV8Value::CreateObject(nullptr, nullptr);
			{
				auto dict = value->GetDictionary();

				std::vector<CefString> keys;
				dict->GetKeys(keys);
				for (auto& e : keys) {
					CefRefPtr<CefV8Value> resultVal;
					ValueToV8Value(resultVal, dict->GetValue(e));

					result->SetValue(e, resultVal, V8_PROPERTY_ATTRIBUTE_NONE);
				}
			}
			break;
		case VTYPE_LIST:
		{
			auto list = value->GetList();
			result = CefV8Value::CreateArray(list->GetSize());

			for (size_t i = 0; i < list->GetSize(); ++i) {
				CefRefPtr<CefV8Value> resultVal;
				ValueToV8Value(resultVal, list->GetValue(i));

				result->SetValue(i, resultVal);
			}
		}
		break;
		}
	}

	class JSApp : public V8HandlerList<JSApp> {
	private:
		class ModuleInfo {
		public:
			int32_t Id;
			CefString Name;
			std::list<std::tuple<int32_t, CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>> StringResCallbacks;
			std::mutex StringResLock;

			ModuleInfo(CefString name, int32_t id) : Name(name), Id(id) {}
		};

		ModuleInfo m_module;
		std::mutex m_moduleLock;

		std::list<std::pair<CefRefPtr<CefV8Context>, std::unordered_map<UniString, CefRefPtr<CefV8Value>>>> m_listeners;
		std::mutex m_listenersLock;
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
				{ "nFireOnExec", &JSApp::nFireOnExec }
			}),
			m_module(moduleName, moduleId)
		{ }

		virtual ~JSApp() {}

		void SetModule(CefString name, int32_t id) {
			std::lock_guard<std::mutex> guard{ m_moduleLock };
			m_module.Name = name;
			m_module.Id = id;
			m_module.StringResCallbacks.clear();
		}

		void ExecuteStringResCallback(const CefString& res, int32_t moduleId, int32_t callbackId) {
			std::lock_guard<std::mutex> guard{ m_moduleLock };
			if (m_module.Id != moduleId)
				return;

			std::lock_guard<std::mutex> guard1{ m_module.StringResLock };

			if (m_module.StringResCallbacks.size() == 0)
				return;
			auto& callbacks = m_module.StringResCallbacks;
			for (auto iter = callbacks.begin(); iter != callbacks.end(); ++iter) {
				if (std::get<0>(*iter) == callbackId) {
					std::tuple<int32_t, CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>> callbackTuple = *iter;
					callbacks.erase(iter);

					CefString resCopy = res;
					std::get<2>(callbackTuple)->GetTaskRunner()->PostTask(new V8ContextFunctorTask([callbackTuple, resCopy]() {
						std::get<1>(callbackTuple)->ExecuteFunction(nullptr, { CefV8Value::CreateString(resCopy) });
					}, std::get<2>(callbackTuple)));

					return;
				}
			}
		}

		void RemoveV8Context(CefRefPtr<CefV8Context> context) {
			{
				std::lock_guard<std::mutex> guard{ m_module.StringResLock };
				m_module.StringResCallbacks.remove_if([&context](auto& e) {
					return context->IsSame(std::get<2>(e));
				});
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

	private:
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
			std::lock_guard<std::mutex> callbacksGuard{ sender.m_module.StringResLock };

			int32_t callbackId = sender.m_module.StringResCallbacks.size();
			sender.m_module.StringResCallbacks.emplace_back(std::make_tuple(callbackId, args[1], CefV8Context::GetCurrentContext()));

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
	};
}