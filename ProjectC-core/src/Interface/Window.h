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
			gcroot<ProjectC_interface::MainWindow^> m_window;
			gcroot<System::Windows::Application^> m_application;
			gcroot<DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>^> m_maximizeHandler{ gcnew DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>(nullptr, *this) };
			gcroot<DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>^> m_shutdownHandler{ gcnew DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>(nullptr, *this) };
			gcroot<DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>^> m_minimizeHandler{ gcnew DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>(nullptr, *this) };
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