#pragma once
#include <cef/include/cef_v8.h>
#include <mutex>
#include <list>

#include "../../UniString.h"
#include "../../Utils/String.h"
#include "../../App.h"
#include "ProcessMessageTypes.h"
#include "FunctorTask.hpp"


namespace ProjectC::Interface::Detail {

	static void V8ValueToString(std::vector<UniString>& strings, CefRefPtr<CefV8Value> value) {
		if (value->IsUndefined()) {
			strings.emplace_back("undefined");
		}
		else if (value->IsNull()) {
			strings.emplace_back("null");
		}
		else if (value->IsArray()) {
			for (int32_t i = 0; i < value->GetArrayLength(); ++i) {
				V8ValueToString(strings, value->GetValue(i));
			}
		}
		else if (value->IsBool()) {
			if (value->GetBoolValue()) {
				strings.emplace_back("true");
			}
			else {
				strings.emplace_back("false");
			}
		}
		else if (value->IsDate()) {
			CefTime time = value->GetDateValue();
			strings.emplace_back(StringUtils::Concatenate(time.hour, ":", time.minute, ":", time.second, "/", time.day_of_month, ".", time.month, ".", time.year));
		}
		else if (value->IsDouble()) {
			strings.emplace_back(std::to_string(value->GetDoubleValue()));
		}
		else if (value->IsFunction()) {
			strings.emplace_back(StringUtils::Concatenate("function ", value->GetFunctionName(), "()"));
		}
		else if (value->IsInt()) {
			strings.emplace_back(std::to_string(value->GetIntValue()));
		}
		else if (value->IsObject()) {
			strings.emplace_back("{ ");
			std::vector<UniString> keys;
			value->GetKeys(keys);
			for (size_t i = 0; i < keys.size(); ++i) {
				strings.emplace_back(StringUtils::Concatenate((i != 0) ? ", " : "", "\"", keys[i], "\" : "));
				V8ValueToString(strings, value->GetValue(keys[i]));
			}
			strings.emplace_back(" }");
		}
		else if (value->IsString()) {
			strings.emplace_back(value->GetStringValue());
		}
		else if (value->IsUInt()) {
			strings.emplace_back(std::to_string(value->GetUIntValue()));
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
			CefString str = StringUtils::Concatenate(time.hour, ":", time.minute, ":", time.second, "/", time.day_of_month, ".", time.month, ".", time.year);
			
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
			LOG_WARN("Converting binary data as CefValue to CefV8Value is forbidden.");
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

	class ModuleObjImpl : public CefV8Accessor, public CefV8Handler {
	private:
		CefString m_name;
		std::list<std::tuple<int32_t, CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>> m_strResCallbacks;
		std::mutex m_strResLock;
		int32_t m_id;
	public:
		ModuleObjImpl(CefString name, int32_t id) : m_name(name), m_id(id) {}

		virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override {
			// sends process message with arguments: BrowserProcessMessageType::ON_EXECUTE, ModuleId(int32_t), Argument(obj)
			if (name == "fireOnExecute") {
				if (arguments.size() < 1) {
					exception = "Argument count is zero.";
					return true;
				}

				auto processMsg = CefProcessMessage::Create("");
				auto argList = processMsg->GetArgumentList();
				argList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::ON_EXECUTE));
				argList->SetInt(1, m_id);
				
				CefRefPtr<CefValue> arg;
				V8ValueToValue(arg, arguments[0]);

				argList->SetValue(2, arg);
				CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
				return true;
			}
			// sends process message with arguments: BrowserProcessMessageType::EXEC_MODULE_LISTENER, ModuleId(int32_t), ListenerKey(CefString), ListenerArg(obj)
			else if (name == "executeModuleListener") {
				if (arguments.size() < 2 || !arguments[0]->IsString()) {
					exception = "Argument count is less than two or first argument is not a string.";
					return true;
				}

				auto processMsg = CefProcessMessage::Create("");
				auto argList = processMsg->GetArgumentList();
				argList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::EXEC_MODULE_LISTENER));
				argList->SetInt(1, m_id);

				CefRefPtr<CefValue> arg = CefValue::Create();
				arg->SetString(arguments[0]->GetStringValue());
				argList->SetValue(2, arg);
				V8ValueToValue(arg, arguments[1]);
				argList->SetValue(3, arg);

				CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
				return true;
			}
			// sends process message with arguments: BrowserProcessMessageType::GET_STR_RESOURCE, ModuleId(int32_t), CallbackId(int32_t), Argument(obj)
			else if (name == "getStringResource") {
				if (arguments.size() < 2 || !arguments[1]->IsFunction()) {
					exception = "Argument count is less than two or second argument is not a function.";
					return true;
				}
				std::lock_guard<std::mutex> guard{ m_strResLock };

				int32_t callbackId = m_strResCallbacks.size();
				m_strResCallbacks.emplace_back(std::make_tuple(callbackId, arguments[1], CefV8Context::GetCurrentContext()));

				auto processMsg = CefProcessMessage::Create("");
				auto argList = processMsg->GetArgumentList();
				argList->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::GET_STR_RESOURCE));
				argList->SetInt(1, m_id);
				argList->SetInt(2, callbackId);

				CefRefPtr<CefValue> arg;
				V8ValueToValue(arg, arguments[0]);
				argList->SetValue(3, arg);

				CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, processMsg);
			}
			return false;
		}

		void ExecuteStringResCallback(const CefString& res, int32_t moduleId, int32_t callbackId) {
			if (m_id != moduleId)
				return;
			std::tuple<int32_t, CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>> callback;
			bool found = false;
			{
				std::lock_guard<std::mutex> guard{ m_strResLock };

				if (m_strResCallbacks.size() == 0)
					return;
				for (auto iter = m_strResCallbacks.begin(); iter != m_strResCallbacks.end(); ++iter) {
					if (std::get<0>(*iter) == callbackId) {
						callback = *iter;
						m_strResCallbacks.erase(iter);
						found = true;
						break;
					}
				}
			}
			if (!found)
				return;

			std::get<2>(callback)->GetTaskRunner()->PostTask(new V8ContextFunctorTask([callback, res]() {
				std::get<1>(callback)->ExecuteFunction(nullptr, { CefV8Value::CreateString(res) });
			}, std::get<2>(callback)));
		}

		void RemoveCallbacksOfV8Context(CefRefPtr<CefV8Context> context) {
			std::lock_guard<std::mutex> guard{ m_strResLock };
			m_strResCallbacks.remove_if([context](std::tuple<int32_t, CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>>& e) {
				return std::get<2>(e)->IsSame(context);
			});
		}

		virtual bool Get(const CefString& name, const CefRefPtr<CefV8Value> object, CefRefPtr<CefV8Value>& retval, CefString& exception) override {
			if (name == "name") {
				retval = CefV8Value::CreateString(m_name);
				return true;
			}
			else if (name == "id") {
				retval = CefV8Value::CreateInt(m_id);
				return true;
			}
			else if (name == "executeModuleListener") {
				retval = CefV8Value::CreateFunction("executeModuleListener", this);
				return true;
			}
			else if (name == "getStringResource") {
				retval = CefV8Value::CreateFunction("getStringResource", this);
				return true;
			}
			else if (name == "fireOnExecute") {
				retval = CefV8Value::CreateFunction("fireOnExecute", this);
				return true;
			}
			return false;
		}

		virtual bool Set(const CefString& name, const CefRefPtr<CefV8Value> object, const CefRefPtr<CefV8Value> value, CefString& exception) override {
			return false;
		}

		int32_t GetId() const {
			return m_id;
		}

		IMPLEMENT_REFCOUNTING(ModuleObjImpl);
	};

	class JSAppObj : public CefV8Handler {
	private:
		std::list<std::pair<CefRefPtr<CefV8Context>, std::unordered_map<UniString, CefRefPtr<CefV8Value>>>> m_listeners;
		std::mutex m_listenersLock;

		CefRefPtr<ModuleObjImpl> m_moduleObj;
		std::mutex m_moduleObjLock;
	public:

		void ResetModuleObject(CefString name, int32_t id) {
			std::lock_guard<std::mutex> guard{ m_moduleObjLock };
			m_moduleObj = nullptr;
			m_moduleObj = new ModuleObjImpl(name, id);
		}

		CefRefPtr<ModuleObjImpl> GetModuleObject() {
			std::lock_guard<std::mutex> guard{ m_moduleObjLock };
			return m_moduleObj;
		}

		void ExecuteJSListener(const CefString& key, CefRefPtr<CefValue> val) {
			std::lock_guard<std::mutex> guard{ m_listenersLock };

			for (auto& e : m_listeners) {
				auto iter = e.second.find(key);
				if (iter != e.second.end()) {

					CefRefPtr<CefV8Context> context = e.first;
					CefRefPtr<CefV8Value> function = iter->second;
					context->GetTaskRunner()->PostTask(new V8ContextFunctorTask([val, function, this]() {
						CefRefPtr<CefV8Value> v8Value;
						ValueToV8Value(v8Value, val);

						function->ExecuteFunction(nullptr, { v8Value });
					}, context));

					e.second.erase(iter);
					return;
				}
			}
		}

		void RemoveListenersOfV8Context(CefRefPtr<CefV8Context> context) {
			std::lock_guard<std::mutex> lock{ m_listenersLock };
			m_listeners.remove_if([context](std::pair<CefRefPtr<CefV8Context>, std::unordered_map<UniString, CefRefPtr<CefV8Value>>>& e) {
				return e.first->IsSame(context);
			});
		}

		/*
		native function doLog();
		native function doGetModule();
		native function doRegisterListener(id, func);
		native function doUnregisterListener(id);
		*/
		virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override
		{
			// sends process message with arguments: BrowserProcessMessageType::LOG, Message(CefString)...
			if (name == "doLog") {
				if (arguments.size() == 0)
					return true;
				CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("");
				CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();
				messageArguments->SetInt(0, static_cast<int32_t>(BrowserProcessMessageType::LOG));

				std::vector<UniString> strings;
				for (auto& e : arguments) {
					V8ValueToString(strings, e);
				}
				for (size_t i = 1; i <= strings.size(); ++i) {
					messageArguments->SetString(i, strings[i]);
				}

				CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(CefProcessId::PID_BROWSER, message);
				return true;
			}
			else if (name == "doGetModule") {
				std::lock_guard<std::mutex> guard{ m_moduleObjLock };
				if (!m_moduleObj) {
					exception = "Module is null.";
					return true;
				}

				retval = CefV8Value::CreateObject(m_moduleObj, nullptr);
				return true;
			}
			else if (name == "doRegisterListener") {
				if (arguments.size() < 2 || !arguments[0]->IsString() || !arguments[1]->IsFunction()) {
					exception = "Invalid arguments (must be string, function).";
					return true;
				}
				CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

				CefString key = arguments[0]->GetStringValue();
				std::unordered_map<UniString, CefRefPtr<CefV8Value>>* listeners{ nullptr };

				// locking for thread safety
				std::lock_guard<std::mutex> guard{ m_listenersLock };
				for (auto& e : m_listeners) {
					if (!listeners && e.first->IsSame(context)) {
						listeners = &e.second;
						continue;
					}
					if (e.second.find(key) != e.second.end()) {
						retval = CefV8Value::CreateBool(false);
						return true;
					}
				}
				if (listeners == nullptr) {
					m_listeners.emplace_back(context, std::unordered_map<UniString, CefRefPtr<CefV8Value>>{});
					listeners = &m_listeners.back().second;
				}

				auto result = listeners->emplace(key, arguments[1]);
				retval = CefV8Value::CreateBool(result.second);
				return true;
			}
			else if (name == "doUnregisterListener") {
				if (arguments.size() < 1 || !arguments[0]->IsString()) {
					exception = "Invalid argument. (must be string).";
					return true;
				}

				CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

				// locking for thread safety
				std::lock_guard<std::mutex> guard{ m_listenersLock };
				for (auto& e : m_listeners) {
					if (e.first->IsSame(context)) {
						auto& listeners = e.second;
						if (listeners.erase(arguments[0]->GetStringValue()) != 0) {
							retval = CefV8Value::CreateBool(true);
							return true;
						}
					}
				}
				retval = CefV8Value::CreateBool(false);
				return true;
			}
			return false;
		}

		IMPLEMENT_REFCOUNTING(JSAppObj);
	};
}