#include "ArchiveManager.h"

std::vector<std::unique_ptr<ProjectC::Execution::IRoutineArchive>> ProjectC::Execution::ArchiveManager::s_archives;


void ProjectC::Execution::ArchiveManager::AddArchive(std::string path)
{
	s_archives.emplace_back(std::make_unique<RoutineArchive>(path, (uint16_t)s_archives.size()));
}

void ProjectC::Execution::ArchiveManager::AddArchive(std::function<IRoutineArchive*(uint16_t id)> factory)
{
	s_archives.emplace_back(factory((uint16_t)s_archives.size()));
}

bool ProjectC::Execution::ArchiveManager::HasRoutine(uint16_t archiveId, uint16_t routineId)
{
	if (archiveId >= s_archives.size())
		return false;
	auto& routines = s_archives[archiveId]->GetRoutines();
	return std::find_if(routines.begin(), routines.end(), [&routineId](const Routine& e) { e.GetId() == routineId; }) != routines.end();
}

ProjectC::Execution::Routine* ProjectC::Execution::ArchiveManager::GetRoutine(uint16_t archiveId, uint16_t routineId)
{
	if (archiveId >= s_archives.size())
		return nullptr;
	auto& routines = s_archives[archiveId]->GetRoutines();
	auto routineIt = std::find_if(routines.begin(), routines.end(), [&routineId](const Routine& e) { e.GetId() == routineId; });
	if (routineIt == routines.end())
		return nullptr;
	return &(*routineIt);
}
