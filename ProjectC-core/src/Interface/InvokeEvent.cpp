#include "InvokeEvent.h"

wxDEFINE_EVENT(ProjectC::Interface::INVOKE_EVENT, ProjectC::Interface::InvokeEvent);

ProjectC::Interface::InvokeEvent::InvokeEvent(Functor functor) : m_functor(functor), wxEvent(wxID_ANY, INVOKE_EVENT)
{ }

wxEvent * ProjectC::Interface::InvokeEvent::Clone() const
{
	return new InvokeEvent(*this);
}

