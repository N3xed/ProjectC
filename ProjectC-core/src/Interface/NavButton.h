#pragma once

#include <wx/button.h>
#include "wx/renderer.h"

namespace ProjectC::Interface {
	class NavButton : public wxButton {
	private:
		wxColour m_background;
		wxColour m_backgroundHover;
		wxColour m_backgroundActive;
	public:
		NavButton(wxWindow* parent, const wxString& text);

		void SetBackground(const wxColour& colour) {
			m_background = colour;
			if (!this->IsMouseInWindow()) {
				SetBackgroundColour(m_background);
			}
		}
		void SetBackgroundHover(const wxColour& colour) {
			m_backgroundHover = colour;
			if (this->IsMouseInWindow()) {
				SetBackgroundColour(m_background);
			}
		}
		void SetBackgroundActive(const wxColour& colour) {
			m_backgroundActive = colour;
		}
		void SetBackgrounds(const wxColour& bg, const wxColour& bgHover, const wxColour& bgActive, const wxColour& fg) {
			SetBackground(bg);
			SetBackgroundHover(bgHover);
			SetBackgroundActive(bgActive);
			SetForegroundColour(fg);
		}
		wxColour& GetBackgroundColourHover() {
			return m_backgroundHover;
		}
		wxColour& GetBackgroundColourActive() {
			return m_backgroundActive;
		}
		wxColour& GetBackground() {
			return m_background;
		}

	private:
		void OnMouseEnter(wxMouseEvent& ev);
		void OnMouseLeave(wxMouseEvent& ev);
		void OnMouseDown(wxMouseEvent& ev);
		void OnMouseUp(wxMouseEvent& ev);
	};
}