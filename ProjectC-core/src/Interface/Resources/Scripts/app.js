var app = (function () {
    native function nLog();
    native function nRegisterListener(id, func);
    native function nUnregisterListener(id, func);

    return{
        // Logs the specified arguments.
        log: function () {
            if(arguments.length > 0)
                nLog(arguments);
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
        module: function () {
            native function nGetModuleName();
            native function nGetModuleId();
            native function nExecModuleListener(key, arg);
            native function nGetStrRes(arg, callback);
            native function nFireOnExec(arg);

            return {
                name: nGetModuleName(),
                id: nGetModuleId(),
                executeModuleListener: nExecModuleListener,
                getStringResource: nGetStrRes,
                fireOnExecute: nFireOnExec
            };
        },
        // Registers a Javascript listener which can be called from native code through: BrowserWindow::ExecuteJSListener
        // app.registerListener(string key, function)
        registerListener: nRegisterListener,
        // Unregisters a previously registers listener and returns true, returns false if no listener was found.
        // app.unregisterListener(string key)
        unregisterListener: nUnregisterListener,
        window: (function () {
            native function nGetWindowTitle();
            native function nSetWindowTitle(str);
            native function nShowWindow(show);
            native function nCloseWindow();
            native function nOpenWindow();

            return {
                setTitle: nSetWindowTitle,
                getTitle: nGetWindowTitle,
                show: nShowWindow,
                close: nCloseWindow,
                open: nOpenWindow
            };
        }())
    };
}());