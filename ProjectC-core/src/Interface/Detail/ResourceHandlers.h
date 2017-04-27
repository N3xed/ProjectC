#pragma once
#include <cef/include/cef_resource_handler.h>

#include "../BrowserWindow.h"
#include "../Resources/ResourceManager.h"
#include "../IGUIModule.h"

namespace ProjectC::Interface::Detail {

	const enum class ResourceType : int32_t {
		UNKNOWN = 0,
		CSS = 1,
		JS = 2
	};

	class ManagerResourceHandler : public CefResourceHandler {
	private:
		ResourceDelegate* m_resDelegate;
	public:
		ManagerResourceHandler(ResourceDelegate* del);

		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;
		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(ManagerResourceHandler);
	};

	class ModuleResourceHandler : public CefResourceHandler
	{
	protected:
		std::shared_ptr<IGUIModule> m_guiModule;
		UniString m_key;
		UniString m_mimeType;

		const uint8_t* m_buffer;
		size_t m_bufferSize;
		size_t m_bufferPos{ 0 };

		IGUIModule::CancelCallback m_cancelCallback;
		ResourceType m_resType;
		std::unique_lock<std::mutex> m_lockGuard;
	public:
		ModuleResourceHandler(std::shared_ptr<IGUIModule> module, UniString key, ResourceType resType);
		ModuleResourceHandler(std::shared_ptr<IGUIModule> module);

		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(ModuleResourceHandler);
	};

	template<ResourceType type>
	class RootResourceHandler : public CefResourceHandler
	{};

	template<>
	class RootResourceHandler<ResourceType::CSS> : public CefResourceHandler {
	public:
		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(RootResourceHandler);
	};

	template<>
	class RootResourceHandler<ResourceType::JS> : public CefResourceHandler {
	public:
		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(RootResourceHandler);
	};

	class RootPageHandler : public CefResourceHandler {
	public:
		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(RootPageHandler);
	};

	class ModulePageHandler : public ModuleResourceHandler {
	public:
		ModulePageHandler(std::shared_ptr<IGUIModule> m_module);

		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;
	};
}