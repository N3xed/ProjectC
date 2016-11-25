#include "Application.h"

ProjectC::IApplication*& ProjectC::GetApplicationInstancePtr() {
	static IApplication* instance;
	return instance;
}

ProjectC::IApplication& ProjectC::GetApp() {
	return *GetApplicationInstancePtr();
}
