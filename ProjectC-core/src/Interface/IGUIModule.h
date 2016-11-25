#pragma once

#include "../String.h"
#include <cstdint>
#include <functional>

class wxDialog;
class wxPanel;

namespace ProjectC {

	class IApplication;

	namespace Interface {

		class IGUIDialog {
		public:
			typedef std::function<void()> ClosedCallback;

			virtual void ShowModal() = 0;
			virtual void Show(ClosedCallback callback = nullptr) = 0;
			virtual int32_t GetReturnCode() = 0;
			virtual bool IsSuccess() = 0;

			virtual void* GetResultData() = 0;
			template<typename T>
			T* GetResultData() {
				return static_cast<T*>(GetResultData());
			}
		};

		class IGUIModule {
		public:
			virtual ~IGUIModule() {}

			virtual std::string GetName() const noexcept = 0;
			virtual void Build(wxPanel* panel, IApplication& app) = 0;
			virtual void OnShow() = 0;
			virtual void OnHide() = 0;

			virtual IGUIDialog* CreateDialog() = 0;
		};
	}
}