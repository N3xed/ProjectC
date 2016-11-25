#pragma once

#include "IFrame.h"
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/string.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <deque>
#include "../Modules/IModule.h"
#include "IGUIModule.h"
#include <wx/aui/aui.h>
#include <boost/bind.hpp>
#include "NavigationArrowPanel.h"
#include <vector>
#include "NavigationElement.h"
#include "StatusBar.h"

namespace ProjectC {
	namespace Interface {
		class ProjectCFrame : public wxFrame, public IFrame {
		private:
			static wxColour s_backgroundColour;
			static wxColour s_fontColour;

			static wxColour s_homeButtonColour;
			static wxColour s_homeButtonActiveColour;
			static wxColour s_homeButtonHoverColour;
			static wxColour s_homeButtonFontColour;
			static wxColour s_homeColour;
			static wxColour s_homeBorderColour;

			static wxColour s_navFontColour;
			static wxColour s_navFontDisabledColour;
			static wxColour s_navColour;
			static wxColour s_navButtonColour;
			static wxColour s_navButtonActiveColour;
			static wxColour s_navButtonHoverColour;
			static wxColour s_navButtonDisabledColour;
			static wxColour s_navButtonDisabledActiveColour;
			static wxColour s_navButtonDisabledHoverColour;
			static wxColour s_navArrowColour;

			static wxColour s_statusBarColour;
			static wxColour s_statusBarFontColour;


			std::deque<NavigationElement> m_navQueue;
			wxPanel* m_navPanel;
			wxPanel* m_controlBarPanel;
			NavButton* m_optionsButton;
			wxPanel* m_homePanel;
			wxBoxSizer* m_contentSizer;
			bool m_isMainWindow;
			StatusBar* m_statusBar;

		public:
			ProjectCFrame(const wxString& title, const wxSize& size, bool isMain = false);
			ProjectCFrame(const ProjectCFrame& frame);
			ProjectCFrame(ProjectCFrame&& frame) = delete;
			~ProjectCFrame();

			virtual void PushLayer(IGUIModule* guiModule, Modules::IModule* module = nullptr) override;
			virtual void PopLayer() override;

			virtual void Hide() override {
				wxFrame::Hide();
			}
			virtual void Show() override {
				wxFrame::Show();
			}
			virtual void BringToFront() override {
				wxFrame::Raise();
			}
			virtual bool IsHomeScreen() const override {
				return m_navQueue.size() == 1;
			}
			virtual std::tuple<IGUIModule *, Modules::IModule *> GetCurrentModule() const override {
				return std::tuple<IGUIModule*, Modules::IModule*>(m_navQueue.back().GUIModule, m_navQueue.back().Module);
			}
			virtual bool IsMainWindow() const override {
				return m_isMainWindow;
			}
			inline NavigationElement& CurrentNavigation() {
				return m_navQueue.back();
			}

			virtual void SetMenuBar(wxMenuBar* menuBar) override {
				wxFrame::SetMenuBar(menuBar);
			}
			virtual void SetNavigationVisible(bool value) override {
				m_controlBarPanel->Show(value);
			}
			virtual bool IsNavigationVisible() const override {
				return m_controlBarPanel->IsShown();
			}

			virtual void SetStatusBarVisible(bool value) override {
				m_statusBar->Show(value);
			}
			virtual void PushStatus(const std::string& text) override {
				m_statusBar->PushStatus(text);
			}
			virtual bool IsStatusBarVisible() const override {
				return m_statusBar->IsShown();
			}
			virtual IStatusBar& GetStatusBar() override {
				return *m_statusBar;
			}

		private:
			void BuildHomePage(wxPanel* panel);
			void BuildControlBar(wxPanel* controlBarPanel);
			
			void PushNavigation(NavigationElement& e, size_t index);
			void RemoveNavigation(NavigationElement& e);
			
			void OnOptionsButtonEvent(wxCommandEvent& ev);
			void OnModuleButton(wxCommandEvent& ev, NavigationElement& navElement);
		};
	}
}