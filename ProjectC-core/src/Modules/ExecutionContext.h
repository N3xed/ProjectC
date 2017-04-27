#pragma once

#include <vector>
#include <mutex>
#include <boost/optional.hpp>
#include <unordered_map>

#include "../UniString.h"
#include "IModule.h"
#include "../IHandle.h"
#include "../Utils/Utils.h"
#include "../IDisposable.h"
#include "ModuleManager.h"
#include "../App.h"


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
		std::unordered_map<UniString, IHandle*> m_userData;
		const ModuleInfo& m_moduleInfo;
		std::mutex m_mutex;
		uint16_t m_id;
	public:
		ExecutionContext(const ModuleInfo& module, uint16_t id);
		~ExecutionContext();

		ModuleManager& GetModuleManager() const {
			return App::ModuleMgr();
		}
		uint16_t GetId() const noexcept {
			return m_id;
		}

		void ExecuteRoutine(uint16_t routineId, Networking::IConnection& con);
		void ExecuteRoutine(Networking::IConnection& con);

		template<typename T> void AddDataUnmanaged(UniString key, T* data) {
			std::lock_guard<std::mutex> lock{ m_mutex };
			m_userData.emplace(key, new IHandleWrapper<false, T>(data));
		}
		template<typename T> void AddDataUnmanaged(UniString key, T& data) {
			AddDataUnmanaged(key, &data);
		}

		template<typename T> void AddDataManaged(UniString key, T* data){
			std::lock_guard<std::mutex> lock{ m_mutex };
			m_userData.emplace(key, new IHandleWrapper<true, T>(data));
		}
		template<typename T>void AddDataManaged(UniString key, const T& data) {
			T* obj = new T(data);
			AddDataManaged(key, obj);
		}
		template<typename T> void AddDataManaged(UniString key, T&& data) {
			T* obj = new T(data);
			AddDataManaged(key, obj);
		}

		template<typename T>
		T* PopDataManaged(const UniString& key) {
			IHandleWrapper<true, T>* obj;
			{
				std::lock_guard<std::mutex> lock{ m_mutex };
				auto it = m_userData.find(key);
				if (it == m_userData.end())
					return nullptr;
				obj = static_cast<IHandleWrapper<true, T>*>(it->second);
				m_userData.erase(it);
			}
			T* result = obj->Pointer;
			obj->Pointer = nullptr;
			delete obj;
			return result;
		}
		template<typename T>
		T* PopDataUnmanaged(const UniString& key) {
			IHandleWrapper<false, T>* obj;
			{
				std::lock_guard<std::mutex> lock{ m_mutex };
				auto it = m_userData.find(key);
				if (it == m_userData.end())
					return nullptr;
				obj = static_cast<IHandleWrapper<false, T>*>(it->second);
				m_userData.erase(it);
			}
			T* result = obj->Pointer;
			delete obj;
			return result;
		}
		void RemoveData(const UniString& key);
		template<bool managed, typename T>
		T* GetData(const UniString& key) {
			std::lock_guard<std::mutex> lock{ m_mutex };
			try {
				return static_cast<IHandleWrapper<managed, T>*>(m_userData.at(key))->Pointer;
			}
			catch (const std::out_of_range&) {
				return nullptr;
			}
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