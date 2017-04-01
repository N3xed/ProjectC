#pragma once
#include <unordered_map>
#include <typeinfo>
#include <functional>

#define FACTORYMGR_FACTORIES(factory) template<> std::unordered_map<const std::type_info*, factory> ProjectC::FactoryManager<factory>::Factories
#define DEFINE_FACTORYMGR(factory) FACTORYMGR_FACTORIES(factory){};


namespace ProjectC {
	template<typename Factory>
	class FactoryManager {
	public:
		static std::unordered_map<const std::type_info*, Factory> Factories;


		static void Add(const std::type_info& type, Factory fact) {
			auto r = Factories.emplace(&type, fact);
			assert(r.second == true);
		}

		static Factory* Get(const std::type_info& type) {
			for (auto& e : Factories) {
				if (*e.first == type)
					return &e.second;
			}
			return nullptr;
		}

		static Factory* Get(const std::string& name) {
			for (auto& e : Factories) {
				if (e.first->name() == name)
					return &e.second;
			}
			return nullptr;
		}

		static Factory* Get(size_t hash_code) {
			for (auto& e : Factories) {
				if (e.first->hash_code() == hash_code)
					return &e.second;
			}
			return nullptr;
		}
	};
}