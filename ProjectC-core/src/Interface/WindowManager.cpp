#include "WindowManager.h"
#include "../App.h"
#include <cef/include/cef_app.h>
#include "Detail/SchemeHandlerFactory.h"
#include <cef/include/cef_task.h>


void ProjectC::Interface::WindowManager::AddWindow(BrowserWindow* window)
{
	std::lock_guard<std::mutex> lock{ m_lock };
	m_windows.emplace_back(window);
}

void ProjectC::Interface::WindowManager::RemoveWindow(BrowserWindow* window)
{
	m_windows.remove_if([window](std::unique_ptr<BrowserWindow>& e) {
		return e.get() == window;
	});
	if (m_windows.empty()) {
		class ExitTask : public CefTask {
		public:
			virtual void Execute() override {
				CefQuitMessageLoop();
			}
			IMPLEMENT_REFCOUNTING(ExitTask);
		};

		CefPostTask(TID_UI, new ExitTask());
	}
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::WindowManager::DoGetWindow(WindowHandle handle)
{
	for (auto& e : m_windows) {
		if (e->GetWindow().GetNativeHandle() == handle) {
			return e.get();
		}
	}
	return nullptr;
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::WindowManager::DoGetWindow(IWindow& window)
{
	return DoGetWindow(window.GetNativeHandle());
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::WindowManager::DoGetWindow(CefRefPtr<CefBrowser> browser)
{
	for (auto& e : m_windows) {
		if (e->GetBrowser().IsSame(browser))
			return e.get();
	}
	return nullptr;
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::WindowManager::GetWindow(WindowHandle handle)
{
	std::lock_guard<std::mutex> guard{ m_lock };
	return DoGetWindow(handle);
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::WindowManager::GetWindow(IWindow& window)
{
	std::lock_guard<std::mutex> guard{ m_lock };
	return DoGetWindow(window);
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::WindowManager::GetWindow(CefRefPtr<CefBrowser> browser)
{
	std::lock_guard<std::mutex> guard{ m_lock };
	return DoGetWindow(browser);
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::WindowManager::GetWindow(size_t index)
{
	std::lock_guard<std::mutex> guard{ m_lock };
	if (index > m_windows.size())
		return nullptr;

	return std::next(m_windows.begin(), index)->get();
}

bool ProjectC::Interface::WindowManager::CloseAllWindows(bool force)
{
	std::vector<BrowserWindow*> tempWindows{};
	bool result = true;
	{
		std::lock_guard<std::mutex> guard{ m_lock };
		tempWindows.reserve(m_windows.size());

		for (auto& e : m_windows) {
			tempWindows.emplace_back(e.get());
		}

		for (auto& e : tempWindows) {
			if (!e->Close(force))
				result = false;
		}
	}

	return result;
}

void ProjectC::Interface::WindowManager::CloseAllWindowsAsync(bool force, std::function<void(bool result)> callback)
{
	class CloseTask : public CefTask {
	public:
		bool _force;
		std::function<void(bool)> _callback;
		CloseTask(bool f, std::function<void(bool)> cbck) : _force(f), _callback(cbck) {}

		virtual void Execute() override {
			bool result = s_instance->CloseAllWindows(_force);
			if (_callback)
				_callback(result);
		}
		IMPLEMENT_REFCOUNTING(CloseTask);
	};

	CefPostTask(TID_UI, new CloseTask(force, callback));
}

ProjectC::Interface::WindowManager::WindowManager()
{ }

void ProjectC::Interface::WindowManager::RegisterSchemeHandlerFactories()
{
	if (!CefRegisterSchemeHandlerFactory(Detail::PageSchemeHandlerFactory::PageSchemeId, "", new Detail::PageSchemeHandlerFactory()))
		LOG_FATAL("Could not register scheme factory.");
	if (!CefRegisterSchemeHandlerFactory(Detail::ResourceSchemeHandlerFactory::ResourceSchemeId, "", new Detail::ResourceSchemeHandlerFactory()))
		LOG_FATAL("Could not register scheme factory.");
}

#if defined(OS_WIN)
#include "Detail/Platform/WindowManagerWin.h"
bool ProjectC::Interface::WindowManager::Initialize()
{
	s_instance = new Detail::WindowManagerWin();
	if (s_instance->PlatformInitialize())
		return true;
	else {
		delete s_instance;
		return false;
	}
}
#else
#error No implementation for this OS.
bool ProjectC::Interface::WindowManager::Initialize()
{
	return false;
}
#endif

void ProjectC::Interface::WindowManager::Shutdown()
{
	delete s_instance;
	s_instance = nullptr;
}

#if defined(OS_WIN)
#include "Detail/Platform/BrowserWindowWin.h"
ProjectC::Interface::BrowserWindow& ProjectC::Interface::WindowManager::CreateBrowserWindow(const UniString& title, int32_t x, int32_t y, int32_t width /*= -1*/, int32_t height /*= -1*/, IWindow* parent /*= nullptr*/)
{
	assert(s_instance);
	Detail::BrowserWindowWin* window = Detail::BrowserWindowWin::Create(title, x, y, width, height, GetModuleHandle(NULL), (parent) ? parent->GetNativeHandle() : nullptr, false);
	if (window == nullptr)
		throw std::exception("Could not create window.");
	s_instance->AddWindow(window);
	return *window;
}

ProjectC::Interface::BrowserWindow& ProjectC::Interface::WindowManager::CreateBrowserWindow(const UniString& title, int32_t width /*= -1*/, int32_t height /*= -1*/, IWindow* parent /*= nullptr*/)
{
	assert(s_instance);
	Detail::BrowserWindowWin* window = Detail::BrowserWindowWin::Create(title, 0, 0, width, height, GetModuleHandle(NULL), (parent) ? parent->GetNativeHandle() : nullptr, true);
	if (window == nullptr)
		throw std::exception("Could not create window.");
	s_instance->AddWindow(window);
	return *window;
}
#else
#error No implementation for this OS.
ProjectC::Interface::IWindow& ProjectC::Interface::WindowManager::CreateBrowserWindow(const UniString& title, int32_t x, int32_t y, int32_t width /*= -1*/, int32_t height /*= -1*/, IWindow* parent /*= nullptr*/)
{
	throw std::exception("No implementation.");
}
ProjectC::Interface::IWindow& ProjectC::Interface::WindowManager::CreateBrowserWindow(const UniString& title, int32_t width /*= -1*/, int32_t height /*= -1*/, IWindow* parent /*= nullptr*/)
{
	throw std::exception("No implementation.");
}

#endif

ProjectC::Interface::WindowManager* ProjectC::Interface::WindowManager::s_instance;
