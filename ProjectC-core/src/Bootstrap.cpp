#include "Bootstrap.h"
#include "ThreadManager.h"
#include "Interface/ProjectCApp.h"
#include "Logger.h"
#include "Interface/Localization/LanguageHelper.h"


void ProjectC::Bootstrap::Initialize(IApplication& app)
{
	app.GetLanguageHelper().Load(Interface::StringResources::English);

	app.GetConnectionManager().EventError().Add([&app](Networking::ConnectionManager& mgr, RoutedException& ex) {
		app.Log(StringUtils::GetErrorMessage<Networking::ConnectionManager>(ex.Name, ex.what()), LogType::FATAL);
	});
	app.Log("Starting...", LogType::NORMAL);
}

void ProjectC::Bootstrap::Uninitialize(IApplication& app)
{

}

void ProjectC::Bootstrap::UnitializeAsync(IApplication& app, std::function<void()> callback)
{
	ThreadManager::Create([&app, callback]() {
		Bootstrap::Uninitialize(app);
		callback();
	});
}
