#pragma once

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/string.h>

namespace ProjectC::Interface {
	class ProjectCFrame : wxFrame {
	public:
		ProjectCFrame(const wxString& title, const wxSize& size);
	};
}