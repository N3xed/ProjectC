#pragma once
#include "../BrowserWindow.h"
#include <cef/include/cef_resource_handler.h>
#include "../Resources/ResourceManager.h"
#include "../IGUIModule.h"

namespace ProjectC::Interface::Detail {
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

	template<int32_t type>
	class ModuleResourceHandler : public CefResourceHandler
	{};

	template<>
	class ModuleResourceHandler<0> : public CefResourceHandler {
		std::shared_ptr<IGUIModule> m_guiModule;
		UniString m_key;
		const UniString* m_content;
		IGUIModule::CancelCallback m_cancelCallback;
	public:
		ModuleResourceHandler(std::shared_ptr<IGUIModule> module, UniString key);

		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(ModuleResourceHandler);
	};
		
	template<>
	class ModuleResourceHandler<1> : public CefResourceHandler {
		std::shared_ptr<IGUIModule> m_guiModule;
		UniString m_key;
		const UniString* m_content;
		IGUIModule::CancelCallback m_cancelCallback;
	public:
		ModuleResourceHandler(std::shared_ptr<IGUIModule> module, UniString key);

		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(ModuleResourceHandler);
	};

	template<int32_t type>
	class RootResourceHandler : public CefResourceHandler
	{};

	template<>
	class RootResourceHandler<0> : public CefResourceHandler {
	public:
		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(RootResourceHandler);
	};

	template<>
	class RootResourceHandler<1> : public CefResourceHandler {
	public:
		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(RootResourceHandler);
	};

	template<int32_t type>
	class RootPageHandler : public CefResourceHandler
	{};

	template<>
	class RootPageHandler<0> : public CefResourceHandler {
	public:
		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(RootPageHandler);
	};

	template<>
	class RootPageHandler<1> : public CefResourceHandler {
		std::shared_ptr<IGUIModule> m_guiModule;
		IGUIModule::CancelCallback m_cancelCallback;
		const UniString* m_content;
	public:
		RootPageHandler(std::shared_ptr<IGUIModule> m_module);

		virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
		virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

		virtual void Cancel() override;

		IMPLEMENT_REFCOUNTING(RootPageHandler);
	};
}