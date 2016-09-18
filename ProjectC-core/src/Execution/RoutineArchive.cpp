#include "RoutineArchive.h"
#include "RoutineManager.h"

ProjectC::Execution::RoutineArchive::RoutineArchive(std::string name)
{
	m_dllHandle = LoadLibrary(name.c_str());
	if (m_dllHandle == NULL)
		throw std::exception(("Library '" + name + "' not found.").c_str());

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

void ProjectC::Execution::RoutineArchive::AddRoutine(std::string symbolName)
{
	Routine::RoutineFunction func = (Routine::RoutineFunction)GetProcAddress(m_dllHandle, symbolName.c_str());
	if (func == NULL) {
		throw std::exception((std::string("Function '") + symbolName + "' not found in library.").c_str());
	}
	m_routines.emplace_back(Routine(RoutineManager::GetNextId(), func, *this));

	RoutineManager::AddRoutine(m_routines.back());
}
