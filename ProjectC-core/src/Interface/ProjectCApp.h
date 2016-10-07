#pragma once

#include <wx/wx.h>

namespace ProjectC::Interface {
	class ProjectCApp : public wxApp {
	public:
		virtual bool OnInit() override;
		virtual int OnExit() override;
	};
}