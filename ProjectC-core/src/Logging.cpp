#include "Logging.h"

ProjectC::Logging::Logger ProjectC::Logging::Logger::s_instance{};

ProjectC::Logging::LogMessage::~LogMessage()
{
	Logger& logger = Logger::GetInstance();
	std::lock_guard<std::mutex> guard{ logger.GetMutex() };
	logger.Log(*this);
}
