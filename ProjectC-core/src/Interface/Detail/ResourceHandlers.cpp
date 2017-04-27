#include "ResourceHandlers.h"
#include "../IGUIModule.h"
#include "../../Utils/String.h"
#include "../../App.h"
#include "../Resources/ResourceManager.h"
#include "../../Logging.h"

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
	if (response_length > INT_MAX) {
		PROJC_LOG(WARN, "Not all data can be copied, because reponse is too big.");
	}
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


// stylesheets
void ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::CSS>::Cancel()
{ }

bool ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::CSS>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	return false;
}

void ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::CSS>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
}

bool ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::CSS>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	return false;
}

// scripts
void ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::JS>::Cancel()
{ }

bool ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::JS>::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	return false;
}

void ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::JS>::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
}

bool ProjectC::Interface::Detail::RootResourceHandler<ProjectC::Interface::Detail::ResourceType::JS>::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	return false;
}

// main page
void ProjectC::Interface::Detail::RootPageHandler::Cancel()
{ }

bool ProjectC::Interface::Detail::RootPageHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	return false;
}

void ProjectC::Interface::Detail::RootPageHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
}

bool ProjectC::Interface::Detail::RootPageHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	return false;
}

ProjectC::Interface::Detail::ModuleResourceHandler::ModuleResourceHandler(std::shared_ptr<IGUIModule> module, UniString key, ResourceType resType) : m_guiModule(module), m_resType(resType), m_key(key)
{ }

ProjectC::Interface::Detail::ModuleResourceHandler::ModuleResourceHandler(std::shared_ptr<IGUIModule> module) : m_guiModule(module)
{ }

bool ProjectC::Interface::Detail::ModuleResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	switch (m_resType) {
	case ResourceType::CSS:
		m_cancelCallback = m_guiModule->LoadCSS([this](const UniString& content, std::mutex& mutex) {
			m_mimeType = "text/css;charset=utf-16";
			m_lockGuard = std::unique_lock<std::mutex>{ mutex };
			m_buffer = reinterpret_cast<const uint8_t*>(content.GetStruct()->str);
			m_bufferSize = content.GetStruct()->length * sizeof(UniString::char_type);
		}, m_key);
		break;
	case ResourceType::JS:
		m_cancelCallback = m_guiModule->LoadJS([this](const UniString& content, std::mutex& mutex) {
			m_mimeType = "test/javascript;charset=utf-16";
			m_lockGuard = std::unique_lock<std::mutex>{ mutex };
			m_buffer = reinterpret_cast<const uint8_t*>(content.GetStruct()->str);
			m_bufferSize = content.GetStruct()->length * sizeof(UniString::char_type);
		}, m_key);
		break;
	case ResourceType::UNKNOWN:
		m_cancelCallback = m_guiModule->LoadResource([this](const uint8_t* buffer, size_t size, const UniString& mimeType, std::mutex& mutex) {
			m_lockGuard = std::unique_lock<std::mutex>{ mutex };
			m_mimeType = mimeType;
			m_buffer = buffer;
			m_bufferSize = size;
		}, m_key);
		break;
	}

	return true;
}

void ProjectC::Interface::Detail::ModuleResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
	response_length = m_bufferSize;
	response->SetMimeType(m_mimeType);
	response->SetStatus(200);
}

bool ProjectC::Interface::Detail::ModuleResourceHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	std::memcpy(data_out, m_buffer + m_bufferPos, bytes_to_read);
	m_bufferPos += bytes_to_read;
	bytes_read = bytes_to_read;

	if (m_bufferSize == m_bufferPos) {
		if (m_lockGuard.owns_lock())
			m_lockGuard.unlock();
	}
	return true;
}

void ProjectC::Interface::Detail::ModuleResourceHandler::Cancel()
{
	if (m_lockGuard.owns_lock())
		m_lockGuard.unlock();
	m_cancelCallback();
}

ProjectC::Interface::Detail::ModulePageHandler::ModulePageHandler(std::shared_ptr<IGUIModule> module) : ModuleResourceHandler(module)
{ }

bool ProjectC::Interface::Detail::ModulePageHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	m_cancelCallback = m_guiModule->LoadHTML([this](const UniString& content, std::mutex& mutex) {
		m_mimeType = "text/html;charset=utf-16";
		m_lockGuard = std::unique_lock<std::mutex>{ mutex };
		m_buffer = reinterpret_cast<const uint8_t*>(content.GetStruct()->str);
		m_bufferSize = content.GetStruct()->length * sizeof(UniString::char_type);
	});
	return true;
}
