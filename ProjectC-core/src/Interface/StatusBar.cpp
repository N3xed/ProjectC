#include "StatusBar.h"



ProjectC::Interface::StatusBar::StatusBar(wxWindow* parent, size_t height) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, height), wxBORDER_NONE)
{
	m_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_leftSizer = new wxBoxSizer(wxHORIZONTAL);
	m_statusText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition);
	m_elementsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	wxBoxSizer* superSizer = new wxBoxSizer(wxHORIZONTAL);
	superSizer->Add(m_statusText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
	
	m_elementsPanel->SetSizer(m_sizer);
	superSizer->Add(m_elementsPanel, 1, wxEXPAND | wxLEFT, 5);

	m_sizer->Add(m_leftSizer, 1, wxEXPAND);

	SetSizer(superSizer);
	SetAutoLayout(true);
}

uint32_t ProjectC::Interface::StatusBar::PushStatus(const std::string& text)
{
	m_statusText->SetLabel(text);
	m_stack.push(text);
	return m_stack.size() - 1;
}

void ProjectC::Interface::StatusBar::AddLeft(wxWindow* widget)
{
	widget->Reparent(m_elementsPanel);
	m_leftSizer->Add(widget, 0, wxALIGN_CENTER_VERTICAL);
}

void ProjectC::Interface::StatusBar::AddLeftSpacer(size_t width)
{
	m_leftSizer->AddSpacer(width);
}

void ProjectC::Interface::StatusBar::AddRightSpacer(size_t width)
{
	m_sizer->AddSpacer(width);
}

void ProjectC::Interface::StatusBar::AddRight(wxWindow* widget)
{
	widget->Reparent(m_elementsPanel);
	m_sizer->Add(widget, 0, wxALIGN_CENTER_VERTICAL);
}

void ProjectC::Interface::StatusBar::Clear()
{
	m_elementsPanel->DestroyChildren();
}

void ProjectC::Interface::StatusBar::PopStatus(uint32_t id)
{
	if (m_stack.size() <= id)
		return;
	if (m_stack.size() == 0) {
		m_statusText->SetLabel(m_defaultStr);
		return;
	}

	while (m_stack.size() != id) {
		m_stack.pop();
	}
	if (m_stack.size() == 0) {
		m_statusText->SetLabel(m_defaultStr);
		return;
	}
	m_statusText->SetLabel(m_stack.top());
}

void ProjectC::Interface::StatusBar::PopStatus()
{
	if (m_stack.size() == 0) {
		m_statusText->SetLabel(m_defaultStr);
		return;
	}
	m_stack.pop();
	m_statusText->SetLabel(m_stack.top());
}
