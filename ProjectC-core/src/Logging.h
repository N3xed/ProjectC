#pragma once

#include <queue>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cef/include/base/cef_logging.h>

#include "InvocationList.h"
#include "UniString.h"
#include "Utils/String.h"

namespace ProjectC::Logging {

	const enum class LogType : uint8_t {
		NORMAL = 0,
		WARN = 1,
		FATAL = 2
	};

	class LogMessage;

	class Logger {
	private:
		static Logger s_instance;

	public:
		typedef InvocationList<void(const LogMessage&)> LogListenerList;
	private:
		LogListenerList m_listeners;
		std::mutex m_mutex;
	public:
		std::mutex& GetMutex() {
			return m_mutex;
		}
		void Log(const LogMessage& log) {
			m_listeners(log);
		}
		LogListenerList& OnLog() {
			return m_listeners;
		}

	public:
		static Logger& GetInstance() { return s_instance; }
	};

	class LogMessage {
	public:
		boost::posix_time::ptime Time;
		LogType Importance;
		UniString Message;

		LogMessage(UniString msg, LogType importance) : Importance(importance), Message(msg), Time(boost::posix_time::second_clock::local_time())
		{ }

		~LogMessage();

		template<LogType type, typename... Args>
		static void Create(char* file, int line, const Args&... args) {
			std::string fileStr{ file };
			// <Windows specific>
			std::string fileName = StringUtils::SubStringAfterLast<char>(fileStr, std::string{ "\\" }, 0).ToString();
			// </>

			LogMessage{ StringUtils::Concatenate('[', fileName, ':', line, "] ", args...), type };
		}
	};

#define PROJC_LOG(type, ...) ProjectC::Logging::LogMessage::Create<ProjectC::Logging::LogType::##type>(__FILE__, __LINE__, __VA_ARGS__)

}