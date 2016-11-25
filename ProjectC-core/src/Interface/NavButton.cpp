#include "NavButton.h"



ProjectC::Interface::NavButton::NavButton(wxWindow* parent, const wxString& text) : wxButton(parent, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT)
{
	Bind(wxEVT_ENTER_WINDOW, &NavButton::OnMouseEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &NavButton::OnMouseLeave, this);
	Bind(wxEVT_LEFT_DOWN, &NavButton::OnMouseDown, this);
	Bind(wxEVT_LEFT_UP, &NavButton::OnMouseUp, this);
}

void ProjectC::Interface::NavButton::OnMouseEnter(wxMouseEvent& ev)
{
	SetBackgroundColour(m_backgroundHover);
	ev.Skip();
}

void ProjectC::Interface::NavButton::OnMouseLeave(wxMouseEvent& ev)
{
	SetBackgroundColour(m_background);
	ev.Skip();
}

void ProjectC::Interface::NavButton::OnMouseDown(wxMouseEvent& ev)
{
	SetBackgroundColour(m_backgroundActive);
	ev.Skip();
}

void ProjectC::Interface::NavButton::OnMouseUp(wxMouseEvent& ev)
{
	SetBackgroundColour(m_backgroundHover);
	ev.Skip();
}

