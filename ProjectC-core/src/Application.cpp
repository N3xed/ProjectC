#include "Application.h"
#include "App.h"
#include <iostream>

ProjectC::IApplication& ProjectC::App::Inst() {
	return Application::GetInstance();
}

ProjectC::Interface::BrowserWindowManager& ProjectC::App::WndMgr() {
	return Interface::BrowserWindowManager::GetInstance();
}

ProjectC::Interface::GUIContext& ProjectC::App::GUICnxt() {
	return Interface::GUIContext::GetInstance();
}

ProjectC::Application* ProjectC::Application::s_instance{ nullptr };

void ProjectC::Application::Exit(bool force)
{
	Interface::BrowserWindowManager::GetInstance().CloseAllWindowsAsync(force);
}

ProjectC::Application::Application(CefMainArgs& args)
{
	s_instance = this;

	

	if (!Interface::GUIContext::Initialize(args)) {
		throw std::exception("Could not initialize interface");
	}
}

ProjectC::Application::~Application()
{
	if (m_connectionMgr.IsOpen<Networking::ProtocolType::TCP>()) {
		m_connectionMgr.Close<Networking::ProtocolType::TCP>();
	}
	if (m_connectionMgr.IsOpen<Networking::ProtocolType::UDP>()) {
		m_connectionMgr.Close<Networking::ProtocolType::UDP>();
	}

	Interface::GUIContext::Shutdown();

	s_instance = nullptr;
}
