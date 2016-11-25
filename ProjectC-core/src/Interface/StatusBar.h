#pragma once

#include <stack>
#include <vector>
#include <wx/wx.h>
#include "IStatusBar.h"

namespace ProjectC::Interface {
	class StatusBar : public wxPanel, public IStatusBar {
	private:
		std::stack<wxString> m_stack;

		wxBoxSizer* m_leftSizer;
		wxBoxSizer* m_sizer;
		wxStaticText* m_statusText;
		wxPanel* m_elementsPanel;
		wxString m_defaultStr;
	public:
		StatusBar(wxWindow* parent, size_t height);

		wxStaticText* GetStatusLabel() {
			return m_statusText;
		}

		void SetDefaultText(wxString str) {
			m_defaultStr = str;
			m_statusText->SetLabel(m_defaultStr);
		}

		virtual uint32_t PushStatus(const std::string& text) override;
		virtual void PopStatus(uint32_t id) override;
		virtual void PopStatus() override;

		virtual void AddLeft(wxWindow* widget) override;
		virtual void AddLeftSpacer(size_t width) override;
		virtual void AddRightSpacer(size_t right) override;
		virtual void AddRight(wxWindow* widget) override;

		virtual void Clear()  override;

		virtual wxWindow* GetContainer() override {
			return m_elementsPanel;
		}
	};
}