#include "Application.h"
#include "App.h"
#include <iostream>

ProjectC::IApplication& ProjectC::App::Inst() {
	return Application::GetInstance();
}

ProjectC::Interface::WindowManager& ProjectC::App::WndMgr() {
	return Interface::WindowManager::GetInstance();
}

ProjectC::Interface::GUIContext& ProjectC::App::GUICnxt() {
	return Interface::GUIContext::GetInstance();
}

ProjectC::Application* ProjectC::Application::s_instance{ nullptr };

void ProjectC::Application::Exit(bool force)
{
	Interface::WindowManager::GetInstance().CloseAllWindowsAsync(force);
}

void ProjectC::Application::Log(const std::string& msg, LogType type /*= static_cast<LogType>(0)*/)
{
	m_logger.Log(msg, type);
}

ProjectC::Application::Application(CefMainArgs& args)
{
	s_instance = this;

	m_logger.SetHandler([](const ILogger::LogInfo& log) {
		LOG(INFO) << log.Message;
	});

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
