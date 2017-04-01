#pragma once

#include "../UniString.h"
#include <sdl/SDL.h>
#include <vector>
#include <functional>
#include "../InvocationList.h"


#define WINDOWPOSITION_DEFAULT SDL_WINDOWPOS_UNDEFINED;
#define	WINDOWPOSITION_DEFAULTEX(displayId) SDL_WINDOWPOS_UNDEFINED_DISPLAY(displayId)
#define WINDOWPOSITION_CENTERED SDL_WINDOWPOS_CENTERED
#define WINDOWPOSITION_CENTEREDEX(displayId) SDL_WINDOWPOS_CENTERED_DISPLAY(displayId)


namespace ProjectC::Interface {

	enum WindowFlags : uint32_t {
		WINDOWFLAGS_FULLSCREEN = SDL_WINDOW_FULLSCREEN,
		WINDOWFLAGS_FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
		WINDOWFLAGS_SHOWN = SDL_WINDOW_SHOWN,
		WINDOWFLAGS_HIDDEN = SDL_WINDOW_HIDDEN,
		WINDOWFLAGS_BORDERLESS = SDL_WINDOW_BORDERLESS,
		WINDOWFLAGS_RESIZABLE = SDL_WINDOW_RESIZABLE,
		WINDOWFLAGS_MINIMIZED = SDL_WINDOW_MINIMIZED,
		WINDOWFLAGS_MAXIMIZED = SDL_WINDOW_MAXIMIZED,
		WINDOWFLAGS_HIGHDPI = SDL_WINDOW_ALLOW_HIGHDPI,
		WINDOWFLAGS_ALWAYS_ON_TOP = SDL_WINDOW_ALWAYS_ON_TOP,
		WINDOWFLAGS_NOT_ON_TASKBAR = SDL_WINDOW_SKIP_TASKBAR,
		WINDOWFLAGS_FOCUS = SDL_WINDOW_INPUT_FOCUS,
		WINDOWFLAGS_TOOLTIP = SDL_WINDOW_TOOLTIP,
		WINDOWFLAGS_POPUP_MENU = SDL_WINDOW_POPUP_MENU,
		WINDOWFLAGS_DEFAULT = WINDOWFLAGS_HIGHDPI |
		WINDOWFLAGS_HIDDEN |
		WINDOWFLAGS_RESIZABLE |
		WINDOWFLAGS_FOCUS
	};
	enum MessageBoxFlags : uint32_t {
		MESSAGEBOXFLAGS_ERROR = SDL_MESSAGEBOX_ERROR,
		MESSAGEBOXFLAGS_WARNING = SDL_MESSAGEBOX_WARNING,
		MESSAGEBOXFLAGS_INFORMATION = SDL_MESSAGEBOX_INFORMATION
	};
	enum MessageBoxButtonFlags : uint32_t {
		MESSAGEBOXBUTTONFLAGS_ENTERKEY_BUTTON = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
		MESSAGEBOXBUTTONFLAGS_ESCAPEKEY_BUTTON = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT
	};
	enum FullscreenMode : uint32_t {
		FULLSCREENMODE_WINDOWED_FULLSCREEN = SDL_WINDOW_FULLSCREEN_DESKTOP,
		FULLSCREENMODE_ACTUAL_FULLSCREEN = SDL_WINDOW_FULLSCREEN,
		FULLSCREENMODE_NO_FULLSCREEN = 0
	};

	struct Point {
		int32_t x;
		int32_t y;
	};
	struct Rect {
		union {
			struct {
				int32_t x, y;
			};
			Point point;
		};
		union {
			struct {
				int32_t width, height;
			};
			Point size;
		};

		Rect() {}
		Rect(int32_t x, int32_t y, int32_t width, int32_t height) : point{ x, y }, size{ width, height } {}
		Rect(const Point& p, const Point& s) : point(p), size(s) {}
	};

	class Window {
	public:
		typedef std::function<bool(Window&)> ClosingHandler;
		typedef HWND NativeWindowHandle;
		
	protected:
		SDL_Window* m_window;
		NativeWindowHandle m_windowHandle;
		bool m_closing{ false };
		ClosingHandler m_closingHandler;
		InvocationList<void(Window&)> m_onClose;

		Window(const std::string& title, int32_t x, int32_t y, int32_t width, int32_t height, WindowFlags flags = WINDOWFLAGS_DEFAULT);
	public:
		struct MessageBoxColorScheme {
			uint32_t backgroundColor;
			uint32_t textColor;
			uint32_t buttonBorderColor;
			uint32_t buttonBackgroundColor;
			uint32_t buttonSelectedColor;
		};
		/// <summary>
		/// Used to set the appearance and the buttons of a message box.
		/// Use <c>Window::ShowMessageBox</c>.
		/// </summary>
		class MessageBoxData {
			friend Window;
		private:
			std::vector<SDL_MessageBoxButtonData> m_buttonData;
			MessageBoxColorScheme m_colorScheme;
			bool m_hasColorScheme{ false };
		public:

			/// <summary>
			/// Adds a button to the message box.
			/// </summary>
			/// <param name="flags">The flags.</param>
			/// <param name="resultId">The id that is returned by the <c>ShowMessageBox</c> function when this button was pressed. 
			/// Its <c>unsigned</c> to force positive ids, because a negative result returned by the <c>ShowMessageBox</c> function indicates an error.</param>
			/// <param name="text">The button text.</param>
			/// <returns>Itself for call chaining.</returns>
			MessageBoxData& AddButton(MessageBoxButtonFlags flags, uint16_t resultId, const char* text);

			/// <summary>
			/// Optional.
			/// Used to set a color scheme for the message box.
			/// </summary>
			/// <param name="scheme">The color scheme.</param>
			/// <returns>Itself for call chaining.</returns>
			MessageBoxData& SetColorScheme(MessageBoxColorScheme scheme);
		};

	public:
		~Window();

		SDL_Window* GetSDLHandle() { return m_window; }
		NativeWindowHandle GetNativeHandle() { return m_windowHandle; }

		void SetTitle(const UniString& str);
		void SetTitle(const std::string& str);

		UniString GetTitle();

		void Raise();
		void Mimimize();
		void Maximize();
		void Restore();
		void Show(bool show = true);

		void SetPosition(int32_t x, int32_t y);
		void SetSize(int32_t width, int32_t height);
		void SetMaxSize(int32_t width, int32_t height);
		void SetMinSize(int32_t width, int32_t height);
		void SetOpacity(float opacity);
		void SetResizable(bool resizable);
		void SetFullscreenMode(FullscreenMode mode);
		void SetMouseCapture(bool capture);

		Point GetPosition();
		Point GetSize();
		Point GetMinSize();
		Point GetMaxSize();
		int32_t GetScreenIndex();
		uint32_t GetId();

		float GetOpacity();
		bool IsResizable();
		bool IsFullscreen();
		FullscreenMode GetFullscreenMode();
		bool IsMouseCaptured();

		/// <summary>
		/// Sets the closing handler, which gets called before OnClose.
		/// Return false to stop the window from closing (only works once for security reasons), return true if it can.
		/// </summary>
		/// <param name="handler">The handler.</param>
		void SetClosingHandler(ClosingHandler handler) { m_closingHandler = handler; }
		/// <summary>
		/// Return a list of event listeners for the OnClose event.
		/// Go to the <c>InvocationList</c> documentation for more information.
		/// </summary>
		/// <returns>The <c>InvocationList</c></returns>
		InvocationList<void(Window&)>& OnClose() { return m_onClose; }

		bool SetModal(Window& parent);

		/// <summary>
		/// Closes the window.
		/// </summary>
		/// <param name="force">if set to <c>false</c> and the <c>ClosingHandler</c> returns false, the close is cancelled. Otherwise the window will be closed.</param>
		/// <returns><c>true</c> if the window will be closed, <c>false</c> if not.</returns>
		bool Close(bool force = false);

		static std::string GetLastError();
		static Window* Create(const std::string& title, int32_t x, int32_t y, int32_t width, int32_t height, WindowFlags flags = WINDOWFLAGS_DEFAULT);

		/// <summary>
		/// Shows a more advanced message box.
		/// </summary>
		/// <param name="flags">The appearance flags.</param>
		/// <param name="title">The title.</param>
		/// <param name="message">The message.</param>
		/// <param name="data">Specifies the buttons, their text and optionally the appearance of this message box.</param>
		/// <param name="parent">Optional: The parent window.</param>
		/// <returns><c>-1<c> on error or the button id specified by the <c>MessageBoxData</c>, whose button the user pressed. All ids are >= 0.</returns>
		static int32_t ShowMessageBox(MessageBoxFlags flags, const std::string& title, const std::string& message, const MessageBoxData& data, Window* parent = nullptr);
		/// <summary>
		/// Shows a simple message box.
		/// </summary>
		/// <param name="flags">The flags for apearance.</param>
		/// <param name="title">The title.</param>
		/// <param name="message">The message.</param>
		/// <param name="parent">Optional: The parent window.</param>
		/// <returns><c>true</c> on success, <c>false</c> on error. Get the last error message with <c>GetLastError()</c>.</returns>
		static bool ShowSimpleMessageBox(MessageBoxFlags flags, const std::string& title, const std::string& message, Window* parent = nullptr);

		static Rect GetScreenBounds(int32_t screenIndex);
	};
}