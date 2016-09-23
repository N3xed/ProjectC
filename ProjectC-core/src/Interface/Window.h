#pragma once
#pragma managed
#using "ProjectC-interface.dll"
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>
#include <string>
#include <functional>
#include <boost/bind.hpp>
#include "DelegateWrapper.h"

namespace ProjectC {
	namespace Interface {

		class Window {
		public:
			typedef void(*WindowHandler)(Window& window);
		private:
			typedef DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&> WindowHandlerDelegateWrapper;

			gcroot<ProjectC_interface::MainWindow^> m_window;
			gcroot<System::Windows::Application^> m_application;
			gcroot<WindowHandlerDelegateWrapper^> m_maximizeHandler{ gcnew WindowHandlerDelegateWrapper(nullptr, *this) };
			gcroot<WindowHandlerDelegateWrapper^> m_shutdownHandler{ gcnew WindowHandlerDelegateWrapper(nullptr, *this) };
			gcroot<WindowHandlerDelegateWrapper^> m_minimizeHandler{ gcnew WindowHandlerDelegateWrapper(nullptr, *this) };
		public:

			Window();
			~Window();

			void SetTitle(std::string title);
			void SetMinimizedHandler(WindowHandler handler);
			void SetMaximizedHandler(WindowHandler handler);
			void SetShutdownHandler(WindowHandler handler);

			void Run();
		};
	}
}