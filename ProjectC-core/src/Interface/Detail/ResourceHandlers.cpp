#include "ResourceHandlers.h"
#include "../IGUIModule.h"
#include "../../Utils/String.h"
#include "../../App.h"
#include "../Resources/ResourceManager.h"

// ManagerResourceHandler
ProjectC::Interface::Detail::ManagerResourceHandler::ManagerResourceHandler(ResourceDelegate* del) : m_resDelegate(del)
{ }

bool ProjectC::Interface::Detail::ManagerResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	callback->Continue();
	return true;
}

void ProjectC::Interface::Detail::ManagerResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
	response_length = static_cast<int64>(m_resDelegate->GetSize());
	response->SetStatus(200);
	response->SetMimeType(m_resDelegate->GetMimeType());
}

bool ProjectC::Interface::Detail::ManagerResourceHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	if (ResourceManager::LoadResource(*m_resDelegate, data_out, bytes_to_read)) {
		bytes_read = bytes_to_read;
		return true;
	}
	return false;
}

void ProjectC::Interface::Detail::ManagerResourceHandler::Cancel()
{ }

// ModuleResourceHandler<0>
// stylesheets
ProjectC::Interface::Detail::ModuleResourceHandler<0>::ModuleResourceHandler(std::shared_ptr<IGUIModule> module, UniString key) : m_guiModule(module), m_key(key)
{ }

void ProjectC::Interface::Detail::ModuleResourceHandler<0>::Cancel()
{
	if (m_cancelCallback)
		m_cancelCallback();
}

bool ProjectC::Interface::Detail::ModuleResourceHandler<0>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	std::memcpy(data_out, m_content->GetStruct()->str, bytes_to_read);
	bytes_read = bytes_to_read;
	return true;
}

void ProjectC::Interface::Detail::ModuleResourceHandler<0>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
	response_length = static_cast<int64>(m_content->GetStruct()->length) * sizeof(UniString::char_type);
	response->SetStatus(200);
	response->SetMimeType("text/css;charset=utf-16");
}

bool ProjectC::Interface::Detail::ModuleResourceHandler<0>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	if (m_cancelCallback = m_guiModule->LoadCSS([callback, this](const UniString& content) {
		m_content = &content;
		callback->Continue();
	}, m_key)) {
		return true;
	}
	return false;
}

// ModuleResourceHandler<1>
// scripts
ProjectC::Interface::Detail::ModuleResourceHandler<1>::ModuleResourceHandler(std::shared_ptr<IGUIModule> module, UniString key) : m_guiModule(module), m_key(key)
{ }

void ProjectC::Interface::Detail::ModuleResourceHandler<1>::Cancel()
{
	if (m_cancelCallback)
		m_cancelCallback();
}

bool ProjectC::Interface::Detail::ModuleResourceHandler<1>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	std::memcpy(data_out, m_content->GetStruct()->str, bytes_to_read);
	bytes_read = bytes_to_read;
	return true;
}

void ProjectC::Interface::Detail::ModuleResourceHandler<1>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
	response_length = static_cast<int64>(m_content->GetStruct()->length) * sizeof(UniString::char_type);
	response->SetStatus(200);
	response->SetMimeType("text/javascript;charset=utf-16");
}

bool ProjectC::Interface::Detail::ModuleResourceHandler<1>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	if (m_cancelCallback = m_guiModule->LoadJS([callback, this](const UniString& content) {
		m_content = &content;
		callback->Continue();
	}, m_key)) {
		return true;
	}
	return false;
}

// RootResourceHandler<0>
// stylesheets
void ProjectC::Interface::Detail::RootResourceHandler<0>::Cancel()
{ }

bool ProjectC::Interface::Detail::RootResourceHandler<0>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	return false;
}

void ProjectC::Interface::Detail::RootResourceHandler<0>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
}

bool ProjectC::Interface::Detail::RootResourceHandler<0>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	return false;
}

// RootResourceHandler<1>
// scripts
void ProjectC::Interface::Detail::RootResourceHandler<1>::Cancel()
{ }

bool ProjectC::Interface::Detail::RootResourceHandler<1>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	return false;
}

void ProjectC::Interface::Detail::RootResourceHandler<1>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
}

bool ProjectC::Interface::Detail::RootResourceHandler<1>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	return false;
}

// RootPageHandler<0>
// main page
void ProjectC::Interface::Detail::RootPageHandler<0>::Cancel()
{ }

bool ProjectC::Interface::Detail::RootPageHandler<0>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	return false;
}

void ProjectC::Interface::Detail::RootPageHandler<0>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
}

bool ProjectC::Interface::Detail::RootPageHandler<0>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	return false;
}

// RootPageHandler<1>
// module
ProjectC::Interface::Detail::RootPageHandler<1>::RootPageHandler(std::shared_ptr<IGUIModule> module) : m_guiModule(module)
{ }

void ProjectC::Interface::Detail::RootPageHandler<1>::Cancel()
{ 
	if(m_cancelCallback)
		m_cancelCallback();
}

bool ProjectC::Interface::Detail::RootPageHandler<1>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	std::memcpy(data_out, m_content->GetStruct()->str, bytes_to_read);
	bytes_to_read = bytes_read;
	return true;
}

void ProjectC::Interface::Detail::RootPageHandler<1>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
	response_length = static_cast<int64>(m_content->GetStruct()->length) * sizeof(UniString::char_type);
	response->SetStatus(200);
	response->SetMimeType("text/html;charset=utf-16");
}

bool ProjectC::Interface::Detail::RootPageHandler<1>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	if (m_cancelCallback = m_guiModule->LoadHTML([this, callback](const UniString& content) {
		m_content = &content;
		callback->Continue();
	})) {
		return true;
	}

	return false;
}