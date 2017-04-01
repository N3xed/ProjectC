#pragma once
#include "../UniString.h"

namespace ProjectC::Interface {
	class IStatusbar {
	public:
		virtual ~IStatusbar() {}

		virtual void Show(bool show = true) = 0;
		void Hide() { Show(false); }

		virtual void PushStatus(const UniString& str) = 0;

		virtual void AddChild(const UniString& htmlCode) = 0;
		virtual void RemoveChilds(const UniString& className) = 0;
		virtual void RemoveChilds(const UniString& attributeName, const UniString& value) = 0;
		virtual void RemoveChild(const UniString& id) = 0;

		virtual void RemoveChilds() = 0;
	};
}