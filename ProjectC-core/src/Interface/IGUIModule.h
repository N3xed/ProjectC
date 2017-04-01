#pragma once

#include "../UniString.h"

#include <cstdint>
#include <functional>
#include <map>

#include <cef/include/cef_values.h>

namespace ProjectC {

	class IApplication;

	namespace Interface {
		class IGUIModule {
		public:
			typedef std::function<void(const UniString& content)> LoadCallback;
			typedef std::function<void(const uint8_t* buffer, size_t size, const UniString& mimeType)> ResourceLoadCallback;
			typedef std::function<void()> CancelCallback;
			typedef std::function<void(CefRefPtr<CefValue> arg)> JSListener;

			virtual ~IGUIModule() {}
			
			/// <summary>
			/// Gets the name with is displayed on the main page.
			/// This string is also availabe through JS code: app.module().name()
			/// </summary>
			/// <returns>The displayed name.</returns>
			virtual const UniString& GetName() = 0;
			/// <summary>
			/// Loads the HTML code for the gui.
			/// </summary>
			/// <param name="callback">A callback which should be called when the requested string is available (may be called in any thread, inside or outside this method).</param>
			/// <returns>
			///		A <c>CancelCallback</c>, which gets called when this load gets canceled. Can be nullptr to indicate failure.
			///		Note: <c>LoadCallback</c> should never be called after <c>CancelCallback</c> has been called.
			/// </returns>
			virtual CancelCallback LoadHTML(LoadCallback callback) = 0;

			/// <summary>
			/// Called after the page was fully loaded, could be called multiple times.
			/// </summary>
			virtual void OnLoad() = 0;
			/// <summary>
			/// Called after the page was unloaded.
			/// Note: This method could be called multiple times, after the last call this object gets destroyed.
			/// </summary>
			virtual void OnUnload() = 0;

			/// <summary>
			/// Loads the CSS code for the gui.
			/// Note: How many times or if this method gets called at all depends on the html code from <c>LoadHTML</c>.
			/// This method gets called when the browser requests a stylesheet resource from any "res:module.[key]" URLs. The [key] part gets passed to this method.
			/// </summary>
			/// <param name="callback">A callback which should be called when the requested string is available (may be called in any thread, inside or outside this method).</param>
			/// <param name="key">A key for identification, which resource should be loaded. Note: More information on resource and GUIModule loading is available in the <c>BrowserWindow::PushLayer</c> documentation.</param>
			/// <returns>
			///		A <c>CancelCallback</c>, which gets called when this load gets canceled. Can be nullptr to indicate failure.
			///		Note: <c>LoadCallback</c> should never be called after <c>CancelCallback</c> has been called.
			/// </returns>
			virtual CancelCallback LoadCSS(LoadCallback callback, const UniString& key) = 0;
			/// <summary>
			/// Loads the JS code for the gui.
			/// Note: How many times or if this method gets called at all depends on the html code from <c>LoadHTML</c>.
			/// This method gets called when the browser requests a script resource from any "res:module.[key]" URLs. The [key] part gets passed to this method.
			/// </summary>
			/// <param name="callback">A callback which should be called when the requested string is available (may be called in any thread, inside or outside this method).</param>
			/// <param name="key">A key for identification, which resource should be loaded. Note: More information on resource and GUIModule loading is available in the <c>BrowserWindow::PushLayer</c> documentation.</param>
			/// <returns>
			///		A <c>CancelCallback</c>, which gets called when this load gets canceled.
			///		Note: <c>LoadCallback</c> should never be called after <c>CancelCallback</c> has been called.
			/// </returns>
			virtual CancelCallback LoadJS(LoadCallback callback, const UniString& key) = 0;
			/// <summary>
			/// Loads any other resource. 
			/// Note: How many times or if this method gets called at all depends on the html or js (through XMLHttpRequest, etc.) code.
			/// This method gets called when the browser requests a (non stylesheet and script) resource from any "res:module.[key]" URLs. The [key] part gets passed to this method.
			/// </summary>
			/// <param name="callback">A callback which should be called when the requested resouces are available (may be called in any thread, inside or outside this method).</param>
			/// <param name="key">A key for identification, which resource should be loaded. Note: More information on resource and GUIModule loading is available in the <c>BrowserWindow::PushLayer</c> documentation.</param>
			/// <returns>
			///		A <c>CancelCallback</c>, which gets called when this load gets canceled. Can be nullptr to indicate failure.
			///		Note: <c>LoadCallback</c> should never be called after <c>CancelCallback</c> has been called.
			/// </returns>
			virtual CancelCallback LoadResource(ResourceLoadCallback callback, const UniString& key) = 0;


			/// <summary>
			/// Gets a map of listeners, which can be called through js code: app.module.executeModuleListener(key, arg)
			/// The default implementation returns nullptr.
			/// All listeners will be called on the GUI thread.
			/// </summary>
			/// <returns>The listeners map. Can also return <c>nullptr</c>.</returns>
			virtual const std::map<UniString, JSListener>* GetJSListenerList() const { return nullptr; }


			/// <summary>
			/// Gets a string resource used in JavaScript code.
			/// A JS call to app.module.getStringResource(arg, callback) executes this method.
			/// First argument gets passed to this method, second argument must be a function, to which the requested string gets passed.
			/// </summary>
			/// <param name="id">The identifier.</param>
			/// <returns>The requested string.</returns>
			virtual const UniString& JSGetStringResource(CefRefPtr<CefValue> argument) noexcept { return ""; }
			/// <summary>
			/// This method can be called through JavaScript code: app.module.fireOnExecute(arg), and will be called on the GUI thread.
			/// The first argument gets passed to this method.
			/// </summary>
			/// <param name="argument">The argument.</param>
			virtual void JSOnExecute(CefRefPtr<CefValue> argument) {}
		};
	}
}