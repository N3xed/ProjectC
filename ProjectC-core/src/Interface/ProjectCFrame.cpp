#include "ProjectCFrame.h"
#include "../Application.h"
#include "Localization/LanguageHelper.h"

wxColour ProjectC::Interface::ProjectCFrame::s_backgroundColour{ 0xaaaaaa };
wxColour ProjectC::Interface::ProjectCFrame::s_fontColour{ 0x222222 };

wxColour ProjectC::Interface::ProjectCFrame::s_homeButtonColour{ 0xcccccc };
wxColour ProjectC::Interface::ProjectCFrame::s_homeButtonActiveColour{ 0xa5a5a5 };
wxColour ProjectC::Interface::ProjectCFrame::s_homeButtonHoverColour{ 0x959595 };
wxColour ProjectC::Interface::ProjectCFrame::s_homeButtonFontColour{ 0xdddddd };
wxColour ProjectC::Interface::ProjectCFrame::s_homeColour{ 0x222222 };
wxColour ProjectC::Interface::ProjectCFrame::s_homeBorderColour{ 0xffffff };

wxColour ProjectC::Interface::ProjectCFrame::s_navFontColour{ 0xf2f2f2 };
wxColour ProjectC::Interface::ProjectCFrame::s_navFontDisabledColour{ 0x111111 };
wxColour ProjectC::Interface::ProjectCFrame::s_navColour{ 0x333333 };
wxColour ProjectC::Interface::ProjectCFrame::s_navButtonColour{ 0x333333 };
wxColour ProjectC::Interface::ProjectCFrame::s_navButtonActiveColour{ 0x666666 };
wxColour ProjectC::Interface::ProjectCFrame::s_navButtonHoverColour{ 0x555555 };
wxColour ProjectC::Interface::ProjectCFrame::s_navButtonDisabledColour{ 0xbababa };
wxColour ProjectC::Interface::ProjectCFrame::s_navButtonDisabledActiveColour{ 0xa5a5a5 };
wxColour ProjectC::Interface::ProjectCFrame::s_navButtonDisabledHoverColour{ 0xa5a5a5 };
wxColour ProjectC::Interface::ProjectCFrame::s_navArrowColour{ 0x888888 };

wxColour ProjectC::Interface::ProjectCFrame::s_statusBarColour{ 0x333333 };
wxColour ProjectC::Interface::ProjectCFrame::s_statusBarFontColour{ 0xf2f2f2 };

ProjectC::Interface::ProjectCFrame::ProjectCFrame(const wxString& title, const wxSize& size, bool isMain) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, size, wxDEFAULT_FRAME_STYLE), m_isMainWindow(isMain)
{	
	SetBackgroundColour(s_backgroundColour);
	SetForegroundColour(s_fontColour);

	m_homePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	BuildHomePage(m_homePanel);

	m_controlBarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 20), wxBORDER_NONE);
	BuildControlBar(m_controlBarPanel);

	m_statusBar = new StatusBar(this, 20);
	m_statusBar->SetBackgroundColour(s_statusBarColour);
	m_statusBar->SetForegroundColour(s_statusBarFontColour);
	m_statusBar->GetStatusLabel()->SetForegroundColour(s_statusBarFontColour);
	m_statusBar->SetDefaultText(App::LangHelper()[StringKeys::READY_MSG]);
	
	//m_auiManager.AddPane(m_controlBarPanel, wxAuiPaneInfo().Direction(wxAUI_DOCK_TOP).Gripper(false).PaneBorder(false).CaptionVisible(false).CloseButton(false).DockFixed(true));
	//m_auiManager.AddPane(m_homePanel, wxAuiPaneInfo().Direction(wxAUI_DOCK_CENTER).Gripper(false).PaneBorder(false).CaptionVisible(false).CloseButton(false).Layer(1).DockFixed(true));
	//m_auiManager.AddPane(m_statusBar, wxAuiPaneInfo().Direction(wxAUI_DOCK_BOTTOM).Gripper(false).PaneBorder(false).CaptionVisible(false).CloseButton(false).DockFixed(true));
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_controlBarPanel, 0, wxEXPAND);

	m_contentSizer = new wxBoxSizer(wxHORIZONTAL);
	m_contentSizer->Add(m_homePanel, 1, wxEXPAND);

	sizer->Add(m_contentSizer, 1, wxEXPAND);
	sizer->Add(m_statusBar, 0, wxEXPAND);
	
	SetSizer(sizer);
	SetAutoLayout(true);
}

ProjectC::Interface::ProjectCFrame::ProjectCFrame(const ProjectCFrame& frame) : wxFrame(nullptr, wxID_ANY, frame.GetTitle()), m_isMainWindow(false)
{
	SetBackgroundColour(s_backgroundColour);
	SetForegroundColour(s_fontColour);

	m_homePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	BuildHomePage(m_homePanel);

	m_navQueue.insert(m_navQueue.end(), frame.m_navQueue.begin(), frame.m_navQueue.end());

	wxPanel* controlBarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 20), wxBORDER_NONE);
	BuildControlBar(controlBarPanel);

	m_statusBar = new StatusBar(this, 20);
	m_statusBar->SetBackgroundColour(s_statusBarColour);
	m_statusBar->SetForegroundColour(s_statusBarFontColour);
	m_statusBar->GetStatusLabel()->SetForegroundColour(s_statusBarFontColour);
	m_statusBar->SetDefaultText(App::LangHelper()[StringKeys::READY_MSG]);

	for (auto& e : m_navQueue) {
		if (e.GUIModule != nullptr) {
			wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			e.GUIModule->Build(panel, GetApp());
			e.Panel = panel;
		}
	}

	bool isHome = m_navQueue.size() == 1;

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_contentSizer = new wxBoxSizer(wxHORIZONTAL);

	sizer->Add(controlBarPanel, 0, wxEXPAND);
	if (isHome) {
		m_contentSizer->Add(m_homePanel, 1, wxEXPAND);
	}
	else {
		m_contentSizer->Add(m_navQueue.back().Panel, 1, wxEXPAND);
	}
	sizer->Add(m_contentSizer, 1, wxEXPAND);
	sizer->Add(m_statusBar, 0, wxEXPAND);

	SetSizer(sizer);
	SetAutoLayout(true);
}

void ProjectC::Interface::ProjectCFrame::BuildHomePage(wxPanel* result)
{
	result->SetBackgroundColour(s_backgroundColour);
	result->SetForegroundColour(s_fontColour);

	wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);

	wxScrolledWindow* scrolledWindow = new wxScrolledWindow(result, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxBORDER_NONE);
	scrolledWindow->SetScrollRate(5, 5);

	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
	fgSizer1->SetFlexibleDirection(wxBOTH);

	wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL, scrolledWindow, App::LangHelper()[StringKeys::INFO_TEXT]);
	infoBox->GetStaticBox()->SetForegroundColour(s_homeColour);
	wxFlexGridSizer* infoFlexGrid = new wxFlexGridSizer(1, 4, 0, 0);
	infoFlexGrid->SetFlexibleDirection(wxBOTH);

	// TODO: Display info

	infoBox->Add(infoFlexGrid, 1, wxEXPAND);
	fgSizer1->Add(infoBox, 1, wxEXPAND);

	wxFlexGridSizer* modulesFlexGrid = new wxFlexGridSizer(1, 3, 10, 10);
	modulesFlexGrid->SetFlexibleDirection(wxBOTH);

	// TODO: layout modules

	fgSizer1->Add(modulesFlexGrid, 1, wxEXPAND);

	scrolledWindow->SetSizer(fgSizer1);
	scrolledWindow->Layout();
	scrolledWindow->SetAutoLayout(true);

	bSizer1->Add(scrolledWindow, 1, wxEXPAND | wxALL, 5);

	result->SetSizer(bSizer1);
	result->SetAutoLayout(true);
	result->Layout();
}

void ProjectC::Interface::ProjectCFrame::BuildControlBar(wxPanel* controlBarPanel)
{
	controlBarPanel->SetBackgroundColour(s_navColour);

	wxBoxSizer* bSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_navPanel = new wxPanel(controlBarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	
	wxBoxSizer* navigationSizer = new wxBoxSizer(wxHORIZONTAL);
	NavButton* homeButton = new NavButton(m_navPanel, App::LangHelper()[StringKeys::HOME_TEXT]);
	homeButton->SetBackgrounds(s_navButtonDisabledColour, s_navButtonDisabledHoverColour, s_navButtonDisabledActiveColour, s_navFontDisabledColour);
	homeButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& ev) {
		if (m_navQueue.size() == 1)
			return;
		size_t count = m_navQueue.size() - 1;
		for (size_t i = 0; i < count; ++i) {
			PopLayer();
		}
	});
	m_navQueue.push_back(NavigationElement(nullptr, nullptr, m_homePanel, homeButton));

	navigationSizer->Add(homeButton, 0, 0);
	m_navPanel->SetSizer(navigationSizer);

	for (auto iter = m_navQueue.begin() + 1; iter != m_navQueue.end(); ++iter) {
		PushNavigation(*iter, iter - m_navQueue.begin());
	}

	m_navPanel->Layout();
	m_navPanel->SetAutoLayout(true);
	bSizer2->Add(m_navPanel, 1, wxEXPAND);

	m_optionsButton = new NavButton(controlBarPanel, App::LangHelper()[StringKeys::OPTIONS_TEXT]);
	m_optionsButton->SetBackgrounds(s_navButtonColour, s_navButtonHoverColour, s_navButtonActiveColour, s_navFontColour);

	bSizer2->Add(m_optionsButton, 0);
	m_optionsButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ProjectCFrame::OnOptionsButtonEvent, this);

	controlBarPanel->SetSizer(bSizer2);
	controlBarPanel->SetAutoLayout(true);
	controlBarPanel->Layout();
}

void ProjectC::Interface::ProjectCFrame::PushNavigation(NavigationElement& e, size_t index)
{
	auto sizer = m_navPanel->GetSizer();

	NavigationArrowPanel* arrow = new NavigationArrowPanel(m_navPanel, s_navArrowColour, wxSize(13, 20));
	arrow->SetBackgroundColour(s_navColour);
	sizer->Add(arrow, 0, 0);

	NavButton* button = new NavButton(m_navPanel, e.GUIModule->GetName());
	button->SetBackgrounds(s_navButtonDisabledColour, s_navButtonDisabledHoverColour, s_navButtonDisabledActiveColour, s_navFontDisabledColour);
	button->Bind(wxEVT_BUTTON, [this, index](wxCommandEvent& ev) {
		if (index == m_navQueue.size() - 1)
			return;
		size_t count = m_navQueue.size() - 1 - index;
		for (size_t i = 0; i < count; ++i) {
			PopLayer();
		}
	});
	e.NavigationButton = button;

	sizer->Add(button, 0, 0);
	sizer->Layout();

	//TODO: Animation
}

void ProjectC::Interface::ProjectCFrame::RemoveNavigation(NavigationElement& e)
{
	//TODO: Animation
	auto sizer = m_navPanel->GetSizer();
	
	e.NavigationButton->Hide();
	sizer->Detach(sizer->GetItemCount() - 1);
	
	auto arrow = sizer->GetChildren().back()->GetWindow();
	arrow->Hide();
	sizer->Detach(sizer->GetItemCount() - 1);
	
	e.NavigationButton->Destroy();
	arrow->Destroy();
}

void ProjectC::Interface::ProjectCFrame::OnOptionsButtonEvent(wxCommandEvent& ev)
{
}

void ProjectC::Interface::ProjectCFrame::OnModuleButton(wxCommandEvent& ev, NavigationElement& navElement)
{
	PushLayer(navElement.GUIModule, navElement.Module);
}

ProjectC::Interface::ProjectCFrame::~ProjectCFrame()
{
}

void ProjectC::Interface::ProjectCFrame::PushLayer(IGUIModule* guiModule, Modules::IModule* module /*= nullptr*/)
{
	auto id = m_statusBar->PushStatus(App::LangHelper()[StringKeys::INITIALIZING_MSG]);
	NavigationElement& last = m_navQueue.back();
	wxPanel* newPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	guiModule->Build(newPanel, GetApp());
	newPanel->Hide();
	newPanel->SetAutoLayout(true);
	newPanel->Layout();

	m_navQueue.back().NavigationButton->SetBackgrounds(s_navButtonColour, s_navButtonHoverColour, s_navButtonActiveColour, s_navFontColour);
	m_navQueue.push_back(NavigationElement(module, guiModule, newPanel));
	
	last.Panel->Hide();
	PushNavigation(m_navQueue.back(), m_navQueue.size() - 1);
	
	m_contentSizer->Clear();
	m_contentSizer->Add(newPanel, 1, wxEXPAND);

	// TODO: Animation

	newPanel->Show();
	m_statusBar->PopStatus(id);
}

void ProjectC::Interface::ProjectCFrame::PopLayer()
{
	if (m_navQueue.size() <= 1)
		throw std::exception("Already at home page.");
	
	NavigationElement& last = m_navQueue.back();
	wxPanel* lastPanel = last.Panel;
	
	RemoveNavigation(last);
	if (last.Module != nullptr && last.Module == (m_navQueue.end() - 2)->Module) {
		m_statusBar->Clear();
	}
	m_navQueue.pop_back();
	m_navQueue.back().NavigationButton->SetBackgrounds(s_navButtonDisabledColour, s_navButtonDisabledHoverColour, s_navButtonDisabledActiveColour, s_navFontDisabledColour);

	lastPanel->Hide();
	m_contentSizer->Clear(true);
	m_contentSizer->Add(m_navQueue.back().Panel, 1, wxEXPAND);
	m_navQueue.back().Panel->Show();
}