var app = (function () {
    native function doLog();
    native function doGetModule();
    native function doRegisterListener(id, func);
    native function doUnregisterListener(id);

    return{
        // Logs the specified arguments.
        log: function() {
            doLog(arguments);
        },
        // Gets the object, which contains all information and functions of the current IGUIModule:
        // |arg| gets passed to the IGUIModule methods.
        // object{
        //  name: string,
        //  id: int,
        //  executeModuleListener: function(string key, arg),
        //  getStringResource: function(arg, function(str) callback),
        //  fireOnExecute: function(arg)
        // }
        module: doGetModule(),
        // Registers a Javascript listener which can be called from native code through: BrowserWindow::ExecuteJSListener
        // app.registerListener(int id, function)
        registerListener: doRegisterListener,
        // Unregisters a previously registers listener and returns true, returns false if no listener was found.
        // app.unregisterListener(int id)
        unregisterListener: doUnregisterListener,
    };
}());