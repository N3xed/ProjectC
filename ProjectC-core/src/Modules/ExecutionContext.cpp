#include "ExecutionContext.h"


ProjectC::Modules::ExecutionContext::ExecutionContext(const ModuleInfo& module, uint16_t id) : m_id(id), m_moduleInfo(module)
{ }

ProjectC::Modules::ExecutionContext::~ExecutionContext()
{
	for (auto& e : m_userData) {
		delete e.second;
		e.second = nullptr;
	}
}

void ProjectC::Modules::ExecutionContext::ExecuteRoutine(uint16_t routineId, Networking::IConnection& con)
{
	auto& lastRoutine = ThreadLocalLastRoutine();
	if (lastRoutine && std::get<0>(*lastRoutine) == routineId) {
		std::get<1>(*lastRoutine)(*this, con);
		return;
	}
	auto routine = m_moduleInfo.GetRoutine(routineId);
	lastRoutine = std::tuple<uint16_t, Modules::IModule::Routine>(routineId, routine);

	routine(*this, con);
}

boost::optional<std::tuple<uint16_t, ProjectC::Modules::IModule::Routine>>& ProjectC::Modules::ExecutionContext::ThreadLocalLastRoutine()
{
	static thread_local boost::optional<std::tuple<uint16_t, Modules::IModule::Routine>> lastRoutine = boost::none;
	return lastRoutine;
}

void ProjectC::Modules::ExecutionContext::RemoveData(const UniString& key)
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	auto it = m_userData.find(key);
	delete it->second;
	m_userData.erase(it);
}

void ProjectC::Modules::ExecutionContext::ExecuteRoutine(Networking::IConnection& con)
{
	auto& lastRoutine = ThreadLocalLastRoutine();
	assert(lastRoutine);
	std::get<1>(*lastRoutine)(*this, con);
}
