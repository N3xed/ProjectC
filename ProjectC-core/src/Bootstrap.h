#pragma once
#include <functional>

namespace ProjectC {
	class IApplication;

	class Bootstrap {
	public:
		static void Initialize(IApplication& app);
		static void Uninitialize(IApplication& app);

		static void UnitializeAsync(IApplication& app, std::function<void()> callback);
	};
}