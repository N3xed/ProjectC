#pragma once
#include "cef/include/cef_browser.h"

#include "ProcessMessageTypes.h"
#include "../IStatusbar.h"

namespace ProjectC::Interface {
	class BrowserWindow;
}

namespace ProjectC::Interface::Detail {
	class StatusbarImpl : public IStatusbar {
	private:
		BrowserWindow& m_browserWindow;
	public:
		StatusbarImpl(BrowserWindow& browserWindow);

		virtual void Show(bool show = true) override;

		virtual void PushStatus(const UniString& str) override;

		virtual void AddChild(const UniString& htmlCode) override;

		virtual void RemoveChilds(const UniString& className) override;
		virtual void RemoveChilds(const UniString& attributeName, const UniString& value) override;
		virtual void RemoveChild(const UniString& id) override;
		virtual void RemoveChilds() override;

	};
}