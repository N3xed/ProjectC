#pragma once

#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/richtext/richtextctrl.h>
#include "../Logger.h"
#include <iosfwd>


namespace ProjectC::Interface {
	class ConsoleFrame : public wxDialog {
	private:
		wxRichTextCtrl* m_textCtrl;

	public:
		ConsoleFrame(wxWindow* parent, wxString title, wxSize size);

		void Log(const ILogger::LogInfo& logInfo);
	};
}
