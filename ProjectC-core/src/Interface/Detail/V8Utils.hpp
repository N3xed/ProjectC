#pragma once
#include <cef/include/cef_v8.h>
#include <mutex>
#include <unordered_map>
#include <boost/bind.hpp>

#include "../../UniString.h"
#include "../../Utils/String.h"
#include "../../Logging.h"
#include "FunctorTask.hpp"

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
			result = CefV8Value::CreateArray(static_cast<int>(list->GetSize()));

			for (size_t i = 0; i < list->GetSize(); ++i) {
				CefRefPtr<CefV8Value> resultVal;
				ValueToV8Value(resultVal, list->GetValue(i));

				result->SetValue(static_cast<int>(i), resultVal);
			}
		}
		break;
		}
	}

	class V8CallbackList {
	private:
		std::unordered_map<int32_t, std::tuple<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>>> m_callbacks;
		std::mutex m_mutex;

	public:
		V8CallbackList() {}

		void AddCallback(int32_t id, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> callback) {
			std::lock_guard<std::mutex> guard{ m_mutex };
			m_callbacks.emplace(id, std::make_tuple(context, callback));
		}

		void RemoveByContext(CefRefPtr<CefV8Context> context) {
			std::lock_guard<std::mutex> guard{ m_mutex };
			for (auto iter = m_callbacks.begin(); iter != m_callbacks.end(); ++iter) {
				if (context->IsSame(std::get<0>(iter->second))) {
					m_callbacks.erase(iter);
				}
			}
		}

		void Clear() {
			std::lock_guard<std::mutex> guard{ m_mutex };
			m_callbacks.clear();
		}

		std::tuple<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>> PopById(int32_t id) {
			std::lock_guard<std::mutex> guard{ m_mutex };
			auto iter = m_callbacks.find(id);
			if (iter == m_callbacks.end())
				return std::make_tuple<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>>(nullptr, nullptr);
			auto result = iter->second;
			m_callbacks.erase(iter);
			return result;
		}

		bool ExecuteById(int32_t id, std::function<void(CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> callback)> handler) {
			std::lock_guard<std::mutex> guard{ m_mutex };
			auto iter = m_callbacks.find(id);
			if (iter == m_callbacks.end())
				return false;
			std::tuple<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>>& callbackTuple = iter->second;
			std::get<0>(callbackTuple)->GetTaskRunner()->PostTask(new V8ContextFunctorTask(
				boost::bind(handler, std::get<0>(callbackTuple), std::get<1>(callbackTuple)),
				std::get<0>(callbackTuple)
			));
			m_callbacks.erase(iter);
			return true;
		}

		std::mutex& GetMutex() {
			return m_mutex;
		}
	};
}