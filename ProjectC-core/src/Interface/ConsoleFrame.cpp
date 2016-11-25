#include "ConsoleFrame.h"
#include "../Application.h"
#include "../Config.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>

ProjectC::Interface::ConsoleFrame::ConsoleFrame(wxWindow* parent, wxString title, wxSize size) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, size, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE)
{
	Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
		if (!evt.CanVeto()) {
			Destroy();
			return;
		}
		evt.Veto();
		Hide();
	});

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_textCtrl = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY | wxBORDER_NONE);
	m_textCtrl->SetDefaultStyle(wxTextAttr(App::Conf().GetOrDefault<uint32_t>(ConfigKeys::INTERFACE_STYLE_CONSOLE_FONTNORMALCOL, 0xffffff)));
	m_textCtrl->SetBackgroundColour(wxColour(App::Conf().GetOrDefault<uint32_t>(ConfigKeys::INTERFACE_STYLE_CONSOLE_BACKGROUNDCOL, 0)));
	sizer->Add(m_textCtrl, 1, wxEXPAND);
	SetAutoLayout(true);
	SetSizer(sizer);
	Layout();
}

void ProjectC::Interface::ConsoleFrame::Log(const ILogger::LogInfo& logInfo)
{
	static wxColour normalCol{ App::Conf().GetOrDefault<uint32_t>(ConfigKeys::INTERFACE_STYLE_CONSOLE_FONTNORMALCOL, 0xffffff) };
	static wxColour warnCol{ App::Conf().GetOrDefault<uint32_t>(ConfigKeys::INTERFACE_STYLE_CONSOLE_FONTWARNCOL, 0xee7600) };
	static wxColour fatalCol{ App::Conf().GetOrDefault<uint32_t>(ConfigKeys::INTERFACE_STYLE_CONSOLE_FONTFATALCOL, 0xff0000) };

	this->Freeze();
	switch (logInfo.Importance) {
	case LogType::NORMAL:
		m_textCtrl->BeginTextColour(normalCol);
		break;
	case LogType::WARN:
		m_textCtrl->BeginTextColour(warnCol);
		break;
	case LogType::FATAL:
		m_textCtrl->BeginTextColour(fatalCol);
		break;
	}

	static std::locale zone{ std::cout.getloc(), new boost::posix_time::time_facet("%x-%X") };

	m_textCtrl->WriteText('[');
	std::stringstream ss;
	ss.imbue(zone);
	ss << logInfo.Time;
	m_textCtrl->WriteText(ss.str());
	m_textCtrl->WriteText("] ");
	m_textCtrl->WriteText(logInfo.Message);
	m_textCtrl->WriteText('\n');
	m_textCtrl->EndTextColour();
	this->Thaw();
}
