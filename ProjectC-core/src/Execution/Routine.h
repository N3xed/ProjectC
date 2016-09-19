#pragma once
#pragma unmanaged

#include <stdint.h>
#include "Context.h"
#include "../Networking/Connection.h"

namespace ProjectC {
	namespace Execution {
		using namespace Networking;

		class RoutineArchive;

		class Routine {
		public:
			typedef void(*RoutineFunction)(Context& context, Connection& con);
		private:
			uint32_t m_id;
			RoutineFunction m_function;
			RoutineArchive& m_archive;

		public:
			Routine(uint32_t id, RoutineFunction funcPtr, RoutineArchive& archive) : m_id(id), m_function(funcPtr), m_archive(archive)
			{}

			uint32_t GetId() const {
				return m_id;
			}

			RoutineArchive& GetArchive() {
				return m_archive;
			}
			RoutineFunction GetRoutine() const {
				return m_function;
			}
		};
	}
}