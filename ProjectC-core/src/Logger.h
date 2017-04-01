#pragma once

#include <queue>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "InvocationList.h"

namespace ProjectC {

	const enum class LogType : uint8_t;

	class ILogger {
	public:
		struct LogInfo {
			boost::posix_time::ptime Time;
			LogType Importance;
			UniString Message;
		};

		virtual std::mutex& GetMutex() = 0;
		virtual void Log(const UniString& str, LogType type) = 0;
	};

	class QueueLogger : public ILogger {
	public:
		typedef std::function<void(const ILogger::LogInfo& log)> Handler;
	private:
		std::queue<LogInfo> m_queue;
		std::mutex m_queueMutex;
		Handler m_handler;
	public:
		QueueLogger(Handler handler = nullptr) : m_handler(handler)
		{}

		void SetHandler(Handler handler) {
			{
				std::lock_guard<std::mutex> guard{ m_queueMutex };
				m_handler = handler;
			}
			HandleLogs();
		}

		virtual std::mutex& GetMutex() {
			return m_queueMutex;
		}

		size_t Length() const {
			return m_queue.size();
		}
		void PopLog() {
			m_queue.pop();
		}

		LogInfo& PeekLog() {
			return m_queue.front();
		}

		virtual void Log(const UniString& str, LogType type) override {
			LogInfo result{};
			result.Importance = type;
			result.Message = str;
			result.Time = boost::posix_time::second_clock::local_time();
			{
				std::lock_guard<std::mutex> lock{ m_queueMutex };
				m_queue.push(std::move(result));
			}
			HandleLogs();
		}

		void HandleLogs() {
			std::lock_guard<std::mutex> guard{ m_queueMutex };
			if (m_queue.size() == 0 || !m_handler)
				return;

			while (!m_queue.empty()) {
				auto& log = m_queue.front();
				m_handler(log);
				m_queue.pop();
			}
		}
	};
}