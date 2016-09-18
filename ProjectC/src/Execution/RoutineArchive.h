#pragma once
#pragma unmanaged

#include <vector>
#include <stdint.h>
#include <windows.h>
#include "Routine.h"

namespace ProjectC {
	namespace Execution {

		class RoutineManager;

		class RoutineArchive {
		private:
			std::vector<Routine> m_routines;
			HINSTANCE m_dllHandle;

			typedef void(*InitFunction)(RoutineArchive& archive);
		public:
			RoutineArchive(std::string name);
			~RoutineArchive();

			void AddRoutine(std::string symbolName);
		};
	}
}