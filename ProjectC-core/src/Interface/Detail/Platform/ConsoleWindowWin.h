#pragma once
#include "../../ConsoleWindow.h"
#include <boost/thread.hpp>
#include <windows.h>

namespace ProjectC::Interface::Detail {
	class ConsoleWindowWin : public ConsoleWindow {
	private:
		static ConsoleWindowWin* s_instance;
	private:
		UniString m_delimiter;

		boost::thread m_thread;
		HANDLE m_inputHandle;
		HANDLE m_outputHandle;
		bool m_running, m_shown;
	public:
		ConsoleWindowWin(const UniString& title, const UniString& delimiter);
		virtual ~ConsoleWindowWin();

		virtual void Show(bool show = true) override;
		virtual void Close() override;

		virtual void Write(const UniString& str, uint32_t fontCol = 0xffffff) override;

		virtual bool IsShown() const override { return m_shown; }

	private:
		void ThreadRoutine();
		static BOOL WINAPI WinHandlerRoutine(DWORD dwCtrlType);
	};
}