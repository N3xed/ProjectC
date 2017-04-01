#pragma once

#include "../../BrowserHandler.h"
#include "BrowserWindowWin.h"

namespace ProjectC::Interface::Detail {
	class BrowserHandlerWin : public BrowserHandler {
	public:
		BrowserHandlerWin(BrowserWindowWin* browserWindow) : BrowserHandler(browserWindow)
		{}
	};
}