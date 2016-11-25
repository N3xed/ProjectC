#pragma once

#include <wx/panel.h>
#include <wx/dcclient.h>

namespace ProjectC::Interface {
	class NavigationArrowPanel : public wxWindow {
	private:
		const wxPoint m_points[3] = {wxPoint(0, 0), wxPoint(4, 4), wxPoint(0, 8)};
		wxBrush m_brush;
	public:
		NavigationArrowPanel(wxWindow* parent, const wxColour& arrowColor, const wxSize& size);

		void SetArrowColour(const wxColour& colour);
	private:
		void OnPaint(wxPaintEvent& ev);
	};
}