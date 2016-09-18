#include "RoutineManager.h"

std::vector<std::unique_ptr<ProjectC::Execution::RoutineArchive>> ProjectC::Execution::RoutineManager::s_archives;
std::vector<ProjectC::Execution::Routine*> ProjectC::Execution::RoutineManager::s_routines;

uint32_t ProjectC::Execution::RoutineManager::GetNextId()
{
	return s_routines.size();
}

void ProjectC::Execution::RoutineManager::AddArchive(std::string path)
{
	s_archives.emplace_back(std::make_unique<RoutineArchive>(path));
}

void ProjectC::Execution::RoutineManager::AddRoutine(Routine& routine)
{
	s_routines.emplace_back(&routine);
}

bool ProjectC::Execution::RoutineManager::HasRoutine(uint32_t id)
{
	return id < s_routines.size();
}

ProjectC::Execution::Routine& ProjectC::Execution::RoutineManager::GetRoutine(uint32_t id)
{
	return *s_routines.at(id);
}
