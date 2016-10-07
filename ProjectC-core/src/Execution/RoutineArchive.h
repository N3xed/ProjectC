#pragma once
#pragma unmanaged

#include <vector>
#include <stdint.h>
#include <windows.h>
#include "Routine.h"

namespace ProjectC {
	namespace Execution {

		class ArchiveManager;

		class IRoutineArchive {
		public:
			virtual ~IRoutineArchive() {}

			virtual std::vector<Routine>& GetRoutines() = 0;
			virtual uint16_t GetId() const = 0;

			virtual void AddRoutine(Routine::RoutineFunction func, uint16_t id) = 0;
		};

		class RoutineArchive : public IRoutineArchive {
		private:
			std::vector<Routine> m_routines;
			uint16_t m_id;
			HINSTANCE m_dllHandle;

			typedef void(*InitFunction)(RoutineArchive& archive);
		public:
			RoutineArchive(std::string path, uint16_t id);
			~RoutineArchive();

			virtual void AddRoutine(Routine::RoutineFunction func, uint16_t id) override;

			virtual std::vector<Routine>& GetRoutines() override;
			virtual uint16_t GetId() const override;

		};
	}
}