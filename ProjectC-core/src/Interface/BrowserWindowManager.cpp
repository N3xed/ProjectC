#include "BrowserWindowManager.h"
#include "../App.h"
#include <cef/include/cef_app.h>
#include "Detail/SchemeHandlerFactory.h"
#include <cef/include/cef_task.h>


ProjectC::Interface::BrowserWindow& ProjectC::Interface::BrowserWindowManager::AddWindow(BrowserWindow* window)
{
	std::lock_guard<std::mutex> guard{ m_lock };
	m_windows.emplace_back(window);
	return *window;
}

void ProjectC::Interface::BrowserWindowManager::RemoveWindow(BrowserWindow* window)
{
	std::lock_guard<std::mutex> guard{ m_lock };
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

ProjectC::Interface::BrowserWindow* ProjectC::Interface::BrowserWindowManager::GetWindow(CefRefPtr<CefBrowser> browser)
{
	std::lock_guard<std::mutex> guard{ m_lock };
	for (auto& e : m_windows) {
		if (e->GetBrowser().IsSame(browser))
			return e.get();
	}
	return nullptr;
}

ProjectC::Interface::BrowserWindow* ProjectC::Interface::BrowserWindowManager::GetWindow(size_t index)
{
	std::lock_guard<std::mutex> guard{ m_lock };
	if (index > m_windows.size())
		return nullptr;

	return std::next(m_windows.begin(), index)->get();
}

bool ProjectC::Interface::BrowserWindowManager::CloseAllWindows(bool force)
{
	std::vector<BrowserWindow*> tempWindows{};
	bool result = true;
	{
		std::lock_guard<std::mutex> guard{ m_lock };
		tempWindows.reserve(m_windows.size());

		for (auto& e : m_windows) {
			tempWindows.emplace_back(e.get());
		}
	}

	for (auto& e : tempWindows) {
		if (!e->Close(force))
			result = false;
	}

	return result;
}

void ProjectC::Interface::BrowserWindowManager::CloseAllWindowsAsync(bool force, std::function<void(bool result)> callback)
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

ProjectC::Interface::BrowserWindowManager::BrowserWindowManager()
{ }

bool ProjectC::Interface::BrowserWindowManager::Initialize()
{
	s_instance = new BrowserWindowManager();
	return true;
}

void ProjectC::Interface::BrowserWindowManager::Shutdown()
{
	delete s_instance;
	s_instance = nullptr;
}

ProjectC::Interface::BrowserWindow& ProjectC::Interface::BrowserWindowManager::CreateWindow(const UniString& title, int32_t x, int32_t y, int32_t width, int32_t height)
{
	// using unique_ptr for exception safety
	auto wnd = std::make_unique<BrowserWindow>(x, y, width, height, title);
	return s_instance->AddWindow(wnd.release());
}

ProjectC::Interface::BrowserWindowManager* ProjectC::Interface::BrowserWindowManager::s_instance;
