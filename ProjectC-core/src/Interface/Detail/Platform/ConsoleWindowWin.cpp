#include "ConsoleWindowWin.h"
#include "../../../App.h"
#include "WindowWin.h"

ProjectC::Interface::Detail::ConsoleWindowWin* ProjectC::Interface::Detail::ConsoleWindowWin::s_instance{ nullptr };

ProjectC::Interface::Detail::ConsoleWindowWin::ConsoleWindowWin(const UniString& title, const UniString& delimiter) : ConsoleWindow(), m_delimiter(delimiter), m_shown(true)
{
	assert(!s_instance);
	s_instance = this;

	if (!::AllocConsole()) {
		LOG_FATAL("AllocConsole failed. ", WindowWin::GetLastErrorString());
		throw std::exception("AllocConsole failed.");
	}
	m_inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	m_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	
	::SetConsoleTitleW(title.ToWString().c_str());
	::SetConsoleCtrlHandler(&ConsoleWindowWin::WinHandlerRoutine, TRUE);

	::SetConsoleMode(m_inputHandle, ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT);
	::SetConsoleMode(m_outputHandle, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

	m_running = true;
	m_thread = boost::thread{ boost::bind(&ConsoleWindowWin::ThreadRoutine, this) };
}

ProjectC::Interface::Detail::ConsoleWindowWin::~ConsoleWindowWin()
{
	s_instance = nullptr;
	Close();
}

void ProjectC::Interface::Detail::ConsoleWindowWin::Show(bool show /*= true*/)
{
	switch (show) {
	case true:
		::ShowWindowAsync(GetConsoleWindow(), SW_SHOW);
		break;
	case false:
		::ShowWindowAsync(GetConsoleWindow(), SW_HIDE);
		break;
	}
	m_shown = show;
}

void ProjectC::Interface::Detail::ConsoleWindowWin::Close()
{
	m_running = false;
	::FreeConsole();
	m_thread.join();
}

void ProjectC::Interface::Detail::ConsoleWindowWin::Write(const UniString& str, uint32_t fontCol)
{
	const cef_string_utf16_t* strStruct = str.GetStruct();
	WriteConsoleW(m_outputHandle, strStruct->str, strStruct->length, NULL, NULL);
}

void ProjectC::Interface::Detail::ConsoleWindowWin::ThreadRoutine()
{
	wchar_t buf[8192];
	DWORD charsRead = 0;
	std::vector<StringUtils::SubStringInfo<UniString::char_type>> subStrings;
	BasicUniString tempStr;

	while (m_running) {
		if (ReadConsoleW(m_inputHandle, buf, 8192, &charsRead, NULL)) {
			StringUtils::SplitString(UniString{ buf, static_cast<size_t>(charsRead - 2), false }, tempStr, m_delimiter, subStrings);
			if (!m_commandReceivedListeners(*this, subStrings)) {
				Write(StringUtils::Concatenate("[Info] Command '", tempStr, "' not found.\n"));
			}
			subStrings.clear();
			tempStr.clear();
		}
		else {
			if(m_running)
				LOG_WARN("ReadConsoleW failed.");
			break;
		}
	}
}

BOOL ProjectC::Interface::Detail::ConsoleWindowWin::WinHandlerRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType) {
	case CTRL_CLOSE_EVENT:
		s_instance->OnClose()(*s_instance);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}