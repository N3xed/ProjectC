#pragma once
#pragma unmanaged

#include "RoutineArchive.h"

namespace ProjectC {
	namespace Execution {
		class ArchiveManager {
		private:
			friend RoutineArchive;

			static std::vector<std::unique_ptr<IRoutineArchive>> s_archives;
		public:
			static void AddArchive(std::string path);
			static void AddArchive(std::function<IRoutineArchive*(uint16_t id)> factory);

			static bool HasRoutine(uint16_t archiveId, uint16_t routineId);
			static Routine* GetRoutine(uint16_t archiveId, uint16_t routineId);
		};
	}
}