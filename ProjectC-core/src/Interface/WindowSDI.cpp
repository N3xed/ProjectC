#include "Window.h"
#include <sdl/SDL_syswm.h>
#include "../App.h"
#include "WindowManager.h"
#include <cassert>


ProjectC::Interface::Window::Window(const std::string& title, int32_t x, int32_t y, int32_t width, int32_t height, WindowFlags flags /*= WINDOWFLAGS_DEFAULT*/)
{
	m_window = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);
	if (m_window == nullptr) {
		ShowSimpleMessageBox(MESSAGEBOXFLAGS_ERROR, "Error", std::string("Could not create window:\n") + SDL_GetError());
		return;
	}
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(m_window, &info)) {
		m_windowHandle = info.info.win.window;
	}
	else {
		ShowSimpleMessageBox(MESSAGEBOXFLAGS_ERROR, "Error", std::string("Could not get WMInfo:\n") + SDL_GetError());
	}
}

ProjectC::Interface::Window::~Window()
{
	m_windowHandle = nullptr;
	SDL_DestroyWindow(m_window);
}

void ProjectC::Interface::Window::SetTitle(const UniString& str)
{
	SDL_SetWindowTitle(m_window, str.ToUTF8().c_str());
}

void ProjectC::Interface::Window::SetTitle(const std::string& str)
{
	SDL_SetWindowTitle(m_window, str.c_str());
}

ProjectC::UniString ProjectC::Interface::Window::GetTitle()
{
	return UniString(SDL_GetWindowTitle(m_window));
}

void ProjectC::Interface::Window::Raise()
{
	SDL_RaiseWindow(m_window);
}

void ProjectC::Interface::Window::Mimimize()
{
	SDL_MinimizeWindow(m_window);
}

void ProjectC::Interface::Window::Maximize()
{
	SDL_MaximizeWindow(m_window);
}

void ProjectC::Interface::Window::Restore()
{
	SDL_RestoreWindow(m_window);
}

void ProjectC::Interface::Window::Show(bool show)
{
	switch (show) {
	case true:
		SDL_ShowWindow(m_window);
		return;
	case false:
		SDL_HideWindow(m_window);
		return;
	}
}

void ProjectC::Interface::Window::SetPosition(int32_t x, int32_t y)
{
	SDL_SetWindowPosition(m_window, x, y);
}

void ProjectC::Interface::Window::SetSize(int32_t width, int32_t height)
{
	SDL_SetWindowSize(m_window, width, height);
}

void ProjectC::Interface::Window::SetMaxSize(int32_t width, int32_t height)
{
	SDL_SetWindowMaximumSize(m_window, width, height);
}

void ProjectC::Interface::Window::SetMinSize(int32_t width, int32_t height)
{
	SDL_SetWindowMinimumSize(m_window, width, height);
}

void ProjectC::Interface::Window::SetOpacity(float opacity)
{
	SDL_SetWindowOpacity(m_window, opacity);
}

void ProjectC::Interface::Window::SetResizable(bool resizable)
{
	SDL_SetWindowResizable(m_window, static_cast<SDL_bool>(resizable));
}

ProjectC::Interface::Point ProjectC::Interface::Window::GetPosition()
{
	Point result;
	SDL_GetWindowPosition(m_window, &result.x, &result.y);
	return result;
}

ProjectC::Interface::Point ProjectC::Interface::Window::GetSize()
{
	Point result;
	SDL_GetWindowSize(m_window, &result.x, &result.y);
	return result;
}

ProjectC::Interface::Point ProjectC::Interface::Window::GetMinSize()
{
	Point result;
	SDL_GetWindowMinimumSize(m_window, &result.x, &result.y);
	return result;
}

ProjectC::Interface::Point ProjectC::Interface::Window::GetMaxSize()
{
	Point result;
	SDL_GetWindowMaximumSize(m_window, &result.x, &result.y);
	return result;
}

float ProjectC::Interface::Window::GetOpacity()
{
	float result;
	SDL_GetWindowOpacity(m_window, &result);
	return result;
}

bool ProjectC::Interface::Window::IsResizable()
{
	return (SDL_GetWindowFlags(m_window) & WINDOWFLAGS_RESIZABLE) != 0;
}

bool ProjectC::Interface::Window::Close(bool force)
{
	m_closing = true;
	if (!force && !m_closing && m_closingHandler && !m_closingHandler(*this)) {
		return false;
	}

	m_onClose(*this);
	Show(false);
	delete this;
	return true;
}

ProjectC::Interface::Window* ProjectC::Interface::Window::Create(const std::string& title, int32_t x, int32_t y, int32_t width, int32_t height, WindowFlags flags /*= WINDOWFLAGS_DEFAULT*/)
{
	auto result = new Window(title, x, y, width, height, flags);
	if (result->m_window == nullptr) {
		delete result;
		GetApp().Log(LogType::FATAL, "Could not create window: ", GetLastError());
		return nullptr;
	}
	assert(WindowManager::s_instance);
	WindowManager::GetInstance().AddWindow(result);
	return result;
}

int32_t ProjectC::Interface::Window::ShowMessageBox(MessageBoxFlags flags, const std::string& title, const std::string& message, const MessageBoxData& data, Window* parent /*= nullptr*/)
{
	SDL_MessageBoxData mbData;
	mbData.buttons = data.m_buttonData.data();
	mbData.numbuttons = static_cast<int>(data.m_buttonData.size());
	mbData.flags = flags;
	mbData.title = title.c_str();
	mbData.message = message.c_str();
	mbData.window = (parent) ? parent->m_window : nullptr;

	int32_t buttonId, result;

	if (data.m_hasColorScheme) {
		SDL_MessageBoxColorScheme scheme = {
			{
				{ static_cast<uint8_t>((data.m_colorScheme.backgroundColor & 0xff0000) >> 16), static_cast<uint8_t>((data.m_colorScheme.backgroundColor & 0xff00) >> 8), static_cast<uint8_t>(data.m_colorScheme.backgroundColor & 0xff) },
				{ static_cast<uint8_t>((data.m_colorScheme.textColor & 0xff0000) >> 16), static_cast<uint8_t>((data.m_colorScheme.textColor & 0xff00) >> 8), static_cast<uint8_t>(data.m_colorScheme.textColor & 0xff) },
				{ static_cast<uint8_t>((data.m_colorScheme.buttonBorderColor & 0xff0000) >> 16), static_cast<uint8_t>((data.m_colorScheme.buttonBorderColor & 0xff00) >> 8), static_cast<uint8_t>(data.m_colorScheme.buttonBorderColor & 0xff) },
				{ static_cast<uint8_t>((data.m_colorScheme.buttonBackgroundColor & 0xff0000) >> 16), static_cast<uint8_t>((data.m_colorScheme.buttonBackgroundColor & 0xff00) >> 8), static_cast<uint8_t>(data.m_colorScheme.buttonBackgroundColor & 0xff) },
				{ static_cast<uint8_t>((data.m_colorScheme.buttonSelectedColor & 0xff0000) >> 16), static_cast<uint8_t>((data.m_colorScheme.buttonSelectedColor & 0xff00) >> 8), static_cast<uint8_t>(data.m_colorScheme.buttonSelectedColor & 0xff) }
			}
		};
		mbData.colorScheme = &scheme;
		result = SDL_ShowMessageBox(&mbData, &buttonId);
	}
	else {
		mbData.colorScheme = nullptr;
		result = SDL_ShowMessageBox(&mbData, &buttonId);
	}

	// return buttonId on success or -1 on failure
	return (result == 0) ? buttonId : -1;
}

bool ProjectC::Interface::Window::ShowSimpleMessageBox(MessageBoxFlags flags, const std::string& title, const std::string& message, Window* parent /*= nullptr*/)
{
	return SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), (parent) ? parent->m_window : nullptr) == 0;
}

int32_t ProjectC::Interface::Window::GetScreenIndex()
{
	return SDL_GetWindowDisplayIndex(m_window);
}

void ProjectC::Interface::Window::SetMouseCapture(bool capture)
{
	SDL_SetWindowGrab(m_window, static_cast<SDL_bool>(capture));
}

bool ProjectC::Interface::Window::IsFullscreen()
{
	uint32_t fullscreen = SDL_GetWindowFlags(m_window);
	return (fullscreen & FULLSCREENMODE_ACTUAL_FULLSCREEN) || (fullscreen & FULLSCREENMODE_WINDOWED_FULLSCREEN);
}

bool ProjectC::Interface::Window::IsMouseCaptured()
{
	return SDL_GetWindowGrab(m_window) == SDL_TRUE;
}

ProjectC::Interface::Rect ProjectC::Interface::Window::GetScreenBounds(int32_t screenId)
{
	SDL_Rect rect;
	SDL_GetDisplayBounds(screenId, &rect);
	return Rect{ rect.x, rect.y, rect.w, rect.h };
}

void ProjectC::Interface::Window::SetFullscreenMode(FullscreenMode mode)
{
	SDL_SetWindowFullscreen(m_window, mode);
}

ProjectC::Interface::FullscreenMode ProjectC::Interface::Window::GetFullscreenMode()
{
	uint32_t fullscreen = SDL_GetWindowFlags(m_window);
	return static_cast<FullscreenMode>((fullscreen & FULLSCREENMODE_ACTUAL_FULLSCREEN) | (fullscreen & FULLSCREENMODE_WINDOWED_FULLSCREEN));
}

bool ProjectC::Interface::Window::SetModal(Window& parent)
{
	return SDL_SetWindowModalFor(m_window, parent.m_window) == 0;
}

std::string ProjectC::Interface::Window::GetLastError()
{
	return std::string(SDL_GetError());
}

uint32_t ProjectC::Interface::Window::GetId()
{
	return SDL_GetWindowID(m_window);
}

ProjectC::Interface::Window::MessageBoxData& ProjectC::Interface::Window::MessageBoxData::AddButton(MessageBoxButtonFlags flags, uint16_t resultId, const char* text)
{
	m_buttonData.emplace_back(SDL_MessageBoxButtonData{ flags, resultId, text });
	return *this;
}

ProjectC::Interface::Window::MessageBoxData& ProjectC::Interface::Window::MessageBoxData::SetColorScheme(MessageBoxColorScheme scheme)
{
	m_colorScheme = scheme;
	m_hasColorScheme = true;
	return *this;
}
