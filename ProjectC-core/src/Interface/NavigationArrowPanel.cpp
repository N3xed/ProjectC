#include "NavigationArrowPanel.h"



ProjectC::Interface::NavigationArrowPanel::NavigationArrowPanel(wxWindow* parent, const wxColour& arrowColor, const wxSize& size) : wxWindow(parent, wxID_ANY, wxDefaultPosition, size)
{
	Bind(wxEVT_PAINT, &NavigationArrowPanel::OnPaint, this);
	m_brush = wxBrush(arrowColor, wxBRUSHSTYLE_SOLID);
	SetSize(13, 25);
}

void ProjectC::Interface::NavigationArrowPanel::OnPaint(wxPaintEvent& ev)
{
	wxPaintDC dc{ this };
	dc.Clear();
	dc.SetBrush(m_brush);
	dc.DrawPolygon(3, m_points, (GetSize().GetX() - m_points[1].x) / 2, (GetSize().GetY() - m_points[2].y) / 2);
}

void ProjectC::Interface::NavigationArrowPanel::SetArrowColour(const wxColour& colour)
{
	m_brush.SetColour(colour);
}
