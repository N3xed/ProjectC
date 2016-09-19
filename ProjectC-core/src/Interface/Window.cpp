#include "Window.h"

ProjectC::Interface::Window::Window() : m_window(gcnew ProjectC_interface::MainWindow()), m_application(gcnew System::Windows::Application())
{
	
}

ProjectC::Interface::Window::~Window()
{}

void ProjectC::Interface::Window::SetTitle(std::string title)
{
	m_window->SetTitle(msclr::interop::marshal_as<System::String^>(title));
}

void ProjectC::Interface::Window::SetMinimizedHandler(WindowHandler handler)
{
	m_minimizeHandler->SetFunction(handler);
	m_window->MinimizedHandler = gcnew ProjectC_interface::MainWindow::WindowHandler(m_minimizeHandler, &DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>::Call);
}

void ProjectC::Interface::Window::SetMaximizedHandler(WindowHandler handler)
{
	m_maximizeHandler->SetFunction(handler);
	m_window->MaximizedHandler = gcnew ProjectC_interface::MainWindow::WindowHandler(m_maximizeHandler, &DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>::Call);
}

void ProjectC::Interface::Window::SetShutdownHandler(WindowHandler handler)
{
	m_shutdownHandler->SetFunction(handler);
	m_window->ShutdownHandler = gcnew ProjectC_interface::MainWindow::WindowHandler(m_shutdownHandler, &DelegateWrapper<WindowHandler, ProjectC_interface::MainWindow^, Window&>::Call);
}

void ProjectC::Interface::Window::Run()
{
	m_application->Run(m_window);
}