#pragma once

#include <wx/event.h>

namespace ProjectC::Interface {
	class InvokeEvent : public wxEvent {
	public:
		typedef std::function<void()> Functor;


	private:
		Functor m_functor;
	public:
		InvokeEvent(Functor functor);

		Functor GetFunctor() const {
			return m_functor;
		}

		virtual wxEvent* Clone() const override;
	};

	wxDECLARE_EVENT(INVOKE_EVENT, InvokeEvent);
}
