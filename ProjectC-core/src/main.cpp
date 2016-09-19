#include "Interface/Window.h"

[System::STAThreadAttribute]
int main() {
	ProjectC::Interface::Window mainWindow{};
	mainWindow.Run();
}