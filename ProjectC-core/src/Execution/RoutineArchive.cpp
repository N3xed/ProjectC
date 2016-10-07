#include "RoutineArchive.h"
#include "ArchiveManager.h"

ProjectC::Execution::RoutineArchive::RoutineArchive(std::string path, uint16_t id) : m_id(id)
{
	m_dllHandle = LoadLibrary(path.c_str());
	if (m_dllHandle == NULL)
		throw std::exception(("Library '" + path + "' not found.").c_str());

	InitFunction initFunc = (InitFunction)GetProcAddress(m_dllHandle, "initialize");
	if (initFunc == NULL) {
		FreeLibrary(m_dllHandle);
		throw std::exception("Function 'void initialize(Archive&)' not found in archive.");
	}
	else {
		initFunc(*this);
	}
}

ProjectC::Execution::RoutineArchive::~RoutineArchive()
{
	FreeLibrary(m_dllHandle);
}

void ProjectC::Execution::RoutineArchive::AddRoutine(Routine::RoutineFunction func, uint16_t id)
{
	if (!func) {
		throw std::exception("Function object is not valid.");
	}
	m_routines.emplace_back(Routine(id, func, *this));
}

std::vector<ProjectC::Execution::Routine>& ProjectC::Execution::RoutineArchive::GetRoutines()
{
	return m_routines;
}

uint16_t ProjectC::Execution::RoutineArchive::GetId() const
{
	return m_id;
}
