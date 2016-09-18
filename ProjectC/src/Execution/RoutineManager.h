#pragma once
#pragma unmanaged

#include "RoutineArchive.h"

namespace ProjectC {
	namespace Execution {
		class RoutineManager {
		private:
			friend RoutineArchive;

			static std::vector<std::unique_ptr<RoutineArchive>> s_archives;
			static std::vector<Routine*> s_routines;

			static uint32_t GetNextId();
		public:
			static void AddArchive(std::string path);
			static void AddRoutine(Routine& routine);

			static bool HasRoutine(uint32_t id);
			static Routine& GetRoutine(uint32_t id);
		};
	}
}