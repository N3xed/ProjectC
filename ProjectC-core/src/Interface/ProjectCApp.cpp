#include "ProjectCApp.h"
#include "../Utils.h"
#include "../Application.h"
#include "../Bootstrap.h"
#include "Localization/LanguageHelper.h"

ProjectC::Interface::ProjectCApp::ProjectCApp() : m_connectionMgr()
{
	GetApplicationInstancePtr() = this;
	Bind(INVOKE_EVENT, &ProjectCApp::handleInvokeEvent, this);

	Bootstrap::Initialize(*this);
}

void ProjectC::Interface::ProjectCApp::ShowNewFrame(Interface::IGUIModule* guiModule, Modules::IModule* module)
{
	ProjectCFrame* frame = new ProjectCFrame(App::LangHelper()[StringKeys::APP_TITLE], wxSize(1024, 720));
	m_frames.push_back(frame);
	auto iter = m_frames.end() - 1;
	frame->Bind(wxEVT_CLOSE_WINDOW, [iter, this](wxCloseEvent& ev) {
		m_frames.erase(iter);
		if(GetFramesCount() == 0 && m_consoleFrame != nullptr)
			m_consoleFrame->Destroy();
		ev.Skip();
	});
	if (guiModule != nullptr)
		frame->PushLayer(guiModule, module);
	frame->Show();
}

void ProjectC::Interface::ProjectCApp::ShowNewFrame(IFrame& from, IGUIModule* guiModule, Modules::IModule* module)
{
	ProjectCFrame* frame = new ProjectCFrame(static_cast<ProjectCFrame&>(from));
	m_frames.push_back(frame);
	auto iter = m_frames.end() - 1;
	frame->Bind(wxEVT_CLOSE_WINDOW, [iter, this](wxCloseEvent& ev) {
		m_frames.erase(iter);
		if (GetFramesCount() == 0 && m_consoleFrame != nullptr)
			m_consoleFrame->Destroy();
		ev.Skip();
	});
	if(guiModule != nullptr)
		frame->PushLayer(guiModule, module);
	frame->Show();
}

void ProjectC::Interface::ProjectCApp::ShowLayer(IFrame& frame, IGUIModule* guiModule, Modules::IModule* module)
{
	for (auto& e : m_frames) {
		if (e->CurrentNavigation().GUIModule == guiModule) {
			e->Raise();
			return;
		}
	}
	frame.PushLayer(guiModule, module);
}

void ProjectC::Interface::ProjectCApp::ShowLayerOrRaiseFrame(bool newFrame, IFrame& frame, IGUIModule* guiModule, Modules::IModule* module /*= nullptr*/)
{
	for (auto& e : m_frames) {
		if (e->CurrentNavigation().GUIModule == guiModule) {
			e->Raise();
			return;
		}
	}
	if (newFrame)
		ShowNewFrame(frame, guiModule, module);
	else
		frame.PushLayer(guiModule, module);
}

bool ProjectC::Interface::ProjectCApp::OnInit()
{
	m_consoleFrame = new ConsoleFrame(NULL, App::LangHelper()[StringKeys::CONSOLE_TITLE], wxSize(800, 600));
	ShowNewFrame();
	m_logger.SetHandler([this](QueueLogger& logger) {
		RunOnUIThread([this, &logger]() {
			m_consoleFrame->Show();
			//m_consoleFrame->Raise();
			while(logger.Length() > 0)
				m_consoleFrame->Log(logger.PopLog());
		});
	});
	return true;
}

int ProjectC::Interface::ProjectCApp::OnExit()
{
	Bootstrap::Uninitialize(*this);
	GetApplicationInstancePtr() = nullptr;
	return 0;
}

std::vector<ProjectC::Interface::IFrame *> ProjectC::Interface::ProjectCApp::GetFrames() const
{
	std::vector<IFrame*> result{};
	std::transform(m_frames.begin(), m_frames.end(), result.begin(), Utils::PointerConverter<ProjectCFrame, IFrame>{});
	return result;
}

void ProjectC::Interface::ProjectCApp::Exit(std::string msg /*= ""*/)
{
	wxExit();
}

void ProjectC::Interface::ProjectCApp::Log(const std::string& msg, LogType type)
{ 
	m_logger.Log(msg, type);
}

void ProjectC::Interface::ProjectCApp::RunOnUIThread(std::function<void()> func)
{
	if (wxThread::IsMain()) {
		try {
			func();
		}
		catch (std::exception& e) {
			Log(StringUtils::GetErrorMessage<ProjectCApp>(typeid(func).name(), e.what(), "(ProjectC::IApplication::RunOnUIThread being used)"), LogType::FATAL);
		}
		return;
	}
	wxQueueEvent(this, new InvokeEvent(func));
}

void ProjectC::Interface::ProjectCApp::handleInvokeEvent(InvokeEvent& ev)
{
	try {
		ev.GetFunctor()();
	}
	catch (std::exception& e) {
		Log(StringUtils::GetErrorMessage<ProjectCApp>(typeid(ev).name(), e.what(), "(ProjectC::IApplication::RunOnUIThread being used)"), LogType::FATAL);
	}
}

wxIMPLEMENT_APP(ProjectC::Interface::ProjectCApp);