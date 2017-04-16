#pragma once
#include <functional>
#include <cef/include/cef_v8.h>
#include <unordered_map>

namespace ProjectC::Interface::Detail {

	template<class T>
	class V8HandlerList : public CefV8Handler {
	public:
		typedef std::function<void(const CefString& name, CefRefPtr<CefV8Value> obj, const CefV8ValueList& args, CefRefPtr<CefV8Value>& retval, CefString& exception, T& sender)> Handler;

	private:
		std::unordered_map<CefString, Handler> m_handlers;

	public:
		V8HandlerList(std::initializer_list<std::pair<const CefString, Handler>> list) : m_handlers(list) {}
		V8HandlerList() {}

		virtual ~V8HandlerList() {}

		bool AddHandler(const CefString& name, Handler handler) {
			return m_handlers.emplace(name, handler).second;
		}

		bool RemoveHandler(const CefString& name) {
			auto iter = m_handlers.find(name);
			if (iter == m_handlers.end())
				return false;
			m_handlers.erase(iter);
			return true;
		}

		virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override
		{
			auto iter = m_handlers.find(name);
			if (iter == m_handlers.end())
				return false;
			iter->second(name, object, arguments, retval, exception, dynamic_cast<T&>(*this));
			return true;
		}

		IMPLEMENT_REFCOUNTING(V8HandlerList);
	};
}