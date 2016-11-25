#pragma once

#include <string>

class wxWindow;

namespace ProjectC::Interface {

	class IStatusBar {
	public:
		virtual uint32_t PushStatus(const std::string& text) = 0;
		virtual void PopStatus(uint32_t id) = 0;
		virtual void PopStatus() = 0;

		virtual void AddLeft(wxWindow* widget) = 0;
		virtual void AddLeftSpacer(size_t width) = 0;
		virtual void AddRight(wxWindow* widget) = 0;
		virtual void AddRightSpacer(size_t width) = 0;

		virtual wxWindow* GetContainer() = 0;

		virtual void Clear() = 0;
	};
}