#pragma once

#include <stdint.h>
#include <functional>
#include <vector>
#include <memory>

#include "../UniString.h"

namespace ProjectC {

	namespace Networking {
		class IConnection;
	}
	namespace Interface {
		class IGUIModule;
	}

	namespace Modules {
		class ExecutionContext;
		
		class IModule {
		public:
			typedef std::function<void(ExecutionContext&, Networking::IConnection&)> Routine;
			typedef std::function<void(uint16_t id, Routine r)> RoutineRegisterer;
			typedef std::array<uint8_t, 128> UID_t;
			typedef std::array<uint32_t, 3> Version_t;

			virtual ~IModule() {}

			virtual bool OnInit(ExecutionContext& context) = 0;
			virtual void OnDestroy(ExecutionContext& context) = 0;
			virtual void OnStartup(ExecutionContext& context) = 0;

			virtual const UniString& GetName() const = 0;
			virtual const UniString& GetDescription() const = 0;
			virtual const Version_t& GetVersion() const = 0;
			virtual const UID_t& GetUID() const = 0;

			virtual void RegisterRoutines(RoutineRegisterer registerer) = 0;

			virtual std::vector<std::shared_ptr<Interface::IGUIModule>> GetGUIModules() = 0;
			virtual std::shared_ptr<Interface::IGUIModule> GetOptionsModule() = 0;
		};
	}
}