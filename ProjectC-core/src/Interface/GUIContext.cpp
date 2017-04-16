#include "GUIContext.h"
#include "Resources/ResourceManager.h"
#include "Detail/SchemeHandlerFactory.h"
#include <mutex>
#include "../Application.h"
#include "Detail/RenderProcessHandler.hpp"

ProjectC::Interface::GUIContext* ProjectC::Interface::GUIContext::s_instance{ nullptr };

CefRefPtr<CefApp> ProjectC::Interface::GUIContext::GetCefAppForSubProcess()
{
	class ProcessAppImpl : public CefApp {
		CefRefPtr<Detail::RenderProcessHandler> m_renderProcessHandler = new Detail::RenderProcessHandler();
	public:
		virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) override {
			DoRegisterCustomScheme(registrar);
		}
		virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
			return m_renderProcessHandler;
		}
		IMPLEMENT_REFCOUNTING(ProcessAppImpl);
	};
	static ProcessAppImpl* processApp;
	if (!processApp)
		processApp = new ProcessAppImpl();
	return processApp;
}

ProjectC::Interface::GUIContext::GUIContext(CefMainArgs& args)
{
	s_instance = this;

	if (!Interface::WindowManager::Initialize()) {
		throw std::exception("Failed to initialize WindowManager");
	}

	CefSettings settings{};
	settings.no_sandbox = true;
	settings.ignore_certificate_errors = STATE_ENABLED;
	if (!CefInitialize(args, settings, s_instance, nullptr)) {
		throw std::exception("Failed to initialize CEF");
	}

	m_consoleWindow = std::unique_ptr<Interface::ConsoleWindow>(Interface::ConsoleWindow::Create("Console"));
	m_consoleWindow->Hide();
	m_consoleWindow->AddCommand("refresh", "Refreshes the browser.", [](Interface::ConsoleWindow& console, const Interface::ConsoleWindow::SubStringVector& args) {
		std::shared_ptr<BrowserWindow> wnd{ nullptr };
		if (args.size() < 2)
			wnd = App::WndMgr().GetWindow(static_cast<size_t>(0));
		else
		{
			try {
				size_t index = static_cast<size_t>(std::stoi(args[1].ToString()));
				if (index < 0 || index > App::WndMgr().GetWindowCount())
					return false;
				wnd = App::WndMgr().GetWindow(static_cast<size_t>(index));
			}
			catch (const std::exception&) {
				return false;
			}
		}
		if (wnd) {
			std::weak_ptr<BrowserWindow> weakPtr = wnd;
			RunOnGUIThread([weakPtr]() {
				auto wndPtr = weakPtr.lock();
				if (wndPtr)
					wndPtr->GetBrowser().Reload();
			});
			return true;
		}
		return false;
	});
	m_consoleWindow->AddCommand("showDevTools", "Shows the development tools window.", [](Interface::ConsoleWindow& console, const Interface::ConsoleWindow::SubStringVector& args) {
		std::shared_ptr<BrowserWindow> wnd{ nullptr };
		if (args.size() < 2)
			wnd = App::WndMgr().GetWindow(static_cast<size_t>(0));
		else
		{
			try {
				size_t index = static_cast<size_t>(std::stoi(args[1].ToString()));
				if (index < 0 || index > App::WndMgr().GetWindowCount())
					return false;
				wnd = App::WndMgr().GetWindow(static_cast<size_t>(index));
			}
			catch (const std::exception&) {
				return false;
			}
		}
		if (wnd) {
			std::weak_ptr<BrowserWindow> weakPtr = wnd;
			RunOnGUIThread([weakPtr]() {
				auto wndPtr = weakPtr.lock();
				if (wndPtr)
					wndPtr->ShowDevTools();
			});
			return true;
		}
		return false;
	});
	m_consoleWindow->AddCommand({ "quit", "exit" }, "Exits the application.", [](Interface::ConsoleWindow& console, const Interface::ConsoleWindow::SubStringVector& args) {
		App::Inst().Exit();
		return true;
	});
	m_consoleWindow->AddCommand("execJSCode", "Executes JavaScript code on the main frame of the browser.", [](Interface::ConsoleWindow& console, const Interface::ConsoleWindow::SubStringVector& args) {
		if (args.size() < 3)
			return false;
		std::shared_ptr<BrowserWindow> wnd{ nullptr };
		if (args.size() < 2)
			wnd = App::WndMgr().GetWindow(static_cast<size_t>(0));
		else
		{
			try {
				size_t index = static_cast<size_t>(std::stoi(args[1].ToString()));
				if (index < 0 || index > App::WndMgr().GetWindowCount())
					return false;
				wnd = App::WndMgr().GetWindow(static_cast<size_t>(index));
			}
			catch (const std::exception&) {
				return false;
			}
		}
		if (wnd) {
			std::weak_ptr<BrowserWindow> weakPtr = wnd;
			std::stringstream ss;
			for (size_t i = 2; i < args.size(); ++i) {
				StringUtils::Concatenate(ss, args[i].ToString());
			}
			UniString str = ss.str();
			RunOnGUIThread([weakPtr, str]() {
				auto wndPtr = weakPtr.lock();
				if (wndPtr)
					wndPtr->ExecuteJSCode(str);
			});
			return true;
		}
		return true;
	});


	auto& logger = Logging::Logger::GetInstance();
	logger.OnLog().Clear();
	logger.OnLog().Add([this](const Logging::LogMessage& msg) {
		LOG(INFO) << msg.Message;

		if (m_consoleWindow) {
			m_consoleWindow->Write(msg.Message);
			m_consoleWindow->Write("\n");

			if (!m_consoleWindow->IsShown())
				m_consoleWindow->Show();
		}
	});
}

ProjectC::Interface::GUIContext::~GUIContext()
{ }

void ProjectC::Interface::GUIContext::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
	DoRegisterCustomScheme(registrar);
}


void ProjectC::Interface::GUIContext::OnContextInitialized()
{
	ResourceManager::GetInstance().AddResourceFromFile("root", "text/html", "D:/Projects/ProjectC/ProjectC-core/src/Interface/Resources/Pages/MainWindow.htm", false);
	ResourceManager::GetInstance().AddResourceFromFile("rootCSS", "text/css", "D:/Projects/ProjectC/ProjectC-core/src/Interface/Resources/Pages/MainWindowStyle.css", false);
	ResourceManager::GetInstance().AddResourceFromFile("rootJS", "text/javascript", "D:/Projects/ProjectC/ProjectC-core/src/Interface/Resources/Scripts/MainWindowScript.js", false);

	if (!CefRegisterSchemeHandlerFactory(Detail::PageSchemeHandlerFactory::PageSchemeId, "", new Detail::PageSchemeHandlerFactory()))
		PROJC_LOG(FATAL, "Could not register scheme factory.");
	if (!CefRegisterSchemeHandlerFactory(Detail::ResourceSchemeHandlerFactory::ResourceSchemeId, "", new Detail::ResourceSchemeHandlerFactory()))
		PROJC_LOG(FATAL, "Could not register scheme factory.");

	BrowserWindow& window = Interface::WindowManager::CreateBrowserWindow("ProjectC", 1024, 720, nullptr);

	window.GetWindow().Show(true);
}

void ProjectC::Interface::GUIContext::DoRegisterCustomScheme(CefRefPtr<CefSchemeRegistrar> registrar)
{
	registrar->AddCustomScheme(Detail::PageSchemeHandlerFactory::PageSchemeId, false, false, false);
	registrar->AddCustomScheme(Detail::ResourceSchemeHandlerFactory::ResourceSchemeId, false, false, false);
}

int32_t ProjectC::Interface::GUIContext::RunMessageLoop()
{
	CefRunMessageLoop();
	return 0;
}

bool ProjectC::Interface::GUIContext::Initialize(CefMainArgs& args)
{
	try {
		new GUIContext(args);
	}
	catch (const std::exception& ex) {
		PROJC_LOG(FATAL, "Could not initialize GUIContext: ", ex.what());
		return false;
	}
	return true;
}

void ProjectC::Interface::GUIContext::Shutdown()
{
	Interface::WindowManager::Shutdown();
	CefShutdown();
	s_instance = nullptr;
}

void ProjectC::Interface::GUIContext::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
{
}

void ProjectC::Interface::GUIContext::RunOnGUIThread(std::function<void()> func)
{
	if (CefCurrentlyOn(TID_UI)) {
		try {
			func();
		}
		catch (const std::exception& ex) {
			PROJC_LOG(WARN, "Exception occurred: ", ex.what());
		}
	}
	else {
		class FunctorTask : public CefTask {
		public:
			std::function<void()> Functor;
			FunctorTask(std::function<void()> functor) : Functor(functor) {}
			virtual void Execute() override {
				try {
					Functor();
				}
				catch (const std::exception& ex) {
					PROJC_LOG(WARN, "Exception occurred: ", ex.what());
				}
			}
			IMPLEMENT_REFCOUNTING(FunctorTask);
		};

		CefPostTask(TID_UI, new FunctorTask(func));
	}
}

