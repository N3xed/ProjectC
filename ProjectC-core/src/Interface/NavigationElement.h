#pragma once

#include "../Modules/IModule.h"
#include "NavButton.h"
#include "wx/panel.h"
#include "IGUIModule.h"

namespace ProjectC::Interface {
	class NavigationElement {
	public:
		Modules::IModule* Module;
		IGUIModule* GUIModule;
		wxPanel* Panel;
		NavButton* NavigationButton;

		NavigationElement(Modules::IModule* module, IGUIModule* guiModule, wxPanel* panel, NavButton* button = nullptr) :
			Module(module), GUIModule(guiModule), Panel(panel), NavigationButton(button)
		{}
	};
}