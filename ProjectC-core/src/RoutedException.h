#pragma once
#include <exception>

namespace ProjectC {
	class RoutedException : public std::exception {
	public:
		void* Sender;
		std::string Name;

		RoutedException(void* sender, std::string name, std::string message) : Sender(sender), Name(name), std::exception(message.c_str())
		{}
		RoutedException(void* sender, std::string name, const std::exception& ex) : Sender(sender), Name(name), std::exception(ex)
		{}

		template<typename T>
		T* GetSender() const {
			return static_cast<T*>(Sender);
		}

		template<typename T>
		static RoutedException Create(T* sender, std::string messgae) {
			return RoutedException(sender, typeid(T).name(), message);
		}
		template<typename T>
		static RoutedException Create(T* sender, const std::exception& ex) {
			return RoutedException(sender, typeid(T).name(), ex);
		}
	};
}