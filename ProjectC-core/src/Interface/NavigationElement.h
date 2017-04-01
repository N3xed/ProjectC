#pragma once

#include "../Modules/IModule.h"
#include "IGUIModule.h"

namespace ProjectC::Interface {
	class NavigationElement {
	public:
		Modules::IModule* Module;
		std::shared_ptr<IGUIModule> GUIModule;
		int32_t GUIModuleId;

		NavigationElement(Modules::IModule* module, std::shared_ptr<IGUIModule> guiModule, int32_t guiModuleId) :
			Module(module), GUIModule(guiModule), GUIModuleId(guiModuleId)
		{}
	};
}