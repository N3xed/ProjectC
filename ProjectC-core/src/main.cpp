#include <windows.h>
#include "Application.h"


int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	CefEnableHighDPISupport();

	CefMainArgs args{ hInstance };

	int32_t result = CefExecuteProcess(args, ProjectC::Interface::GUIContext::GetCefAppForSubProcess(), nullptr);
	if (result >= 0) {
		return result;
	}

	try
	{
		new ProjectC::Application(args);
	}
	catch (const std::exception& ex) {
		LOG(FATAL) << ex.what() << std::endl;
		delete &ProjectC::App::Inst();
		return -1;
	}

	result = ProjectC::Interface::GUIContext::GetInstance().RunMessageLoop();
	delete &ProjectC::App::Inst();

	return result;
}