#pragma once

#include <vector>
#include <mutex>
#include "IModule.h"
#include "../IHandle.h"
#include <map>
#include <boost/optional.hpp>
#include "../Utils.h"
#include "../IDisposable.h"
#include "ModuleManager.h"
#include "../Application.h"


namespace ProjectC::Networking {
	class IConnection;
}

namespace ProjectC::Modules {

	class ExecutionContext {
	private:
		template<bool managed, typename T>
		struct IHandleWrapper {};
		template<typename T>
		struct IHandleWrapper<true, T> {
			T* Pointer;
			IHandleWrapper(T* ptr) : Pointer(ptr) {}
			~IHandleWrapper() {
				delete Pointer;
			}
		};
		template<typename T>
		struct IHandleWrapper<false, T> {
			T* Pointer;
			IHandleWrapper(T* ptr) : Pointer(ptr) {}
		};

	private:
		std::map<std::string, IHandle*> m_userData;
		const ModuleInfo& m_moduleInfo;
		std::mutex m_userDataMutex;
		uint16_t m_id;
	public:
		ExecutionContext(const ModuleInfo& module, uint16_t id);
		~ExecutionContext();

		IApplication& GetApplication() const {
			return GetApplication();
		}
		ModuleManager& GetModuleManager() const {
			return GetApplication().GetModuleManager();
		}
		uint16_t GetId() const noexcept {
			return m_id;
		}

		void ExecuteRoutine(uint16_t routineId, Networking::IConnection& con);
		void ExecuteRoutine(Networking::IConnection& con);

		template<typename T> void AddDataUnmanaged(std::string key, T* data) {
			std::lock_guard<std::mutex> lock{ m_userDataMutex };
			m_userData.emplace(key, new IHandleWrapper<false, T>(data));
		}
		template<typename T> void AddDataUnmanaged(std::string key, T& data) {
			AddDataUnmanaged(key, &data);
		}

		template<typename T> void AddDataManaged(std::string key, T* data){
			std::lock_guard<std::mutex> lock{ m_userDataMutex };
			m_userData.emplace(key, new IHandleWrapper<true, T>(data));
		}
		template<typename T>void AddDataManaged(std::string key, const T& data) {
			T* obj = new T(data);
			AddDataManaged(key, obj);
		}
		template<typename T> void AddDataManaged(std::string key, T&& data) {
			T* obj = new T(data);
			AddDataManaged(key, obj);
		}

		template<typename T>
		T* PopDataManaged(const std::string& key) {
			IHandleWrapper<true, T>* obj;
			{
				std::lock_guard<std::mutex> lock{ m_userDataMutex };
				auto it = m_userData.find(key);
				obj = static_cast<IHandleWrapper<true, T>*>(it->second);
				m_userData.erase(it);
			}
			T* result = obj->Pointer;
			obj->Pointer = nullptr;
			delete obj;
			return result;
		}
		template<typename T>
		T* PopDataUnmanaged(const std::string& key) {
			IHandleWrapper<false, T>* obj;
			{
				std::lock_guard<std::mutex> lock{ m_userDataMutex };
				auto it = m_userData.find(key);
				obj = static_cast<IHandleWrapper<false, T>*>(it->second);
				m_userData.erase(it);
			}
			T* result = obj->Pointer;
			delete obj;
			return result;
		}
		void RemoveData(const std::string& key);
		template<bool managed, typename T>
		T* GetData(const std::string& key) {
			std::lock_guard<std::mutex> lock{ m_userDataMutex };
			return static_cast<IHandleWrapper<managed, T>*>(m_userData.at(key))->Pointer;
		}
		const IModule& GetModule() const noexcept {
			return m_moduleInfo.GetModule();
		}
		const ModuleInfo& GetModuleInfo() const noexcept {
			return m_moduleInfo;
		}

	private:
		boost::optional<std::tuple<uint16_t, IModule::Routine>>& ThreadLocalLastRoutine();
	};
}