#pragma once

#include <queue>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "InvocationList.h"

namespace ProjectC {

	const enum class LogType : uint8_t {
		NORMAL = 0,
		WARN = 1,
		FATAL = 2
	};

	class ILogger {
	public:
		struct LogInfo {
			boost::posix_time::ptime Time;
			LogType Importance;
			std::string Message;
		};

		virtual void Log(const std::string& str, LogType type) = 0;
	};

	class QueueLogger : public ILogger {
	public:
		typedef std::function<void(QueueLogger& logger)> Handler;
	private:
		std::queue<LogInfo> m_queue;
		std::mutex m_queueMutex;
		Handler m_handler;
	public:
		QueueLogger(Handler handler = nullptr) : m_handler(handler)
		{}

		void SetHandler(Handler handler) {
			m_handler = handler;
			if (handler && m_queue.size() > 0)
				handler(*this);
		}

		size_t Length() const {
			return m_queue.size();
		}
		LogInfo PopLog() {
			std::lock_guard<std::mutex> lock{ m_queueMutex };
			LogInfo result = m_queue.front();
			m_queue.pop();
			return result;
		}

		virtual void Log(const std::string& str, LogType type) override {
			std::lock_guard<std::mutex> lock{ m_queueMutex };
			LogInfo result{};
			result.Importance = type;
			result.Message = str;
			result.Time = boost::posix_time::second_clock::local_time();
			m_queue.push(std::move(result));
			if (m_handler)
				m_handler(*this);
		}
	};
}