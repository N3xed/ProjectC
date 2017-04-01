#pragma once
#include <cef/include/cef_task.h>
#include <cef/include/cef_v8.h>
#include <functional>

namespace ProjectC::Interface::Detail {
	class FunctorTask : public CefTask {
	private:
		std::function<void()> m_func;

	public:
		FunctorTask(std::function<void()> func) : m_func(func) {}

		virtual void Execute() override
		{
			m_func();
		}

		IMPLEMENT_REFCOUNTING(FunctorTask);
	};

	class V8ContextFunctorTask : public CefTask {
	private:
		std::function<void()> m_func;
		CefRefPtr<CefV8Context> m_context;

	public:
		V8ContextFunctorTask(std::function<void()> func, CefRefPtr<CefV8Context> context) : m_func(func), m_context(context) {}

		virtual void Execute() override
		{
			if (!m_context->Enter()) {
				LOG(WARNING) << "Could not enter V8Context." << std::endl;
				return;
			}
			m_func();
			m_context->Exit();
		}

		IMPLEMENT_REFCOUNTING(V8ContextFunctorTask);
	};
}