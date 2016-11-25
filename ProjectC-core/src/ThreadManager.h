#pragma once

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <functional>

namespace ProjectC {
	const enum class ThreadType: uint32_t {
		MISC = 0,
		GUI = 1,
		NETWORKING = 2,
		MODULES = 3
	};

	class ThreadManager {
	public:
		template<ThreadType groupId, typename Functor>
		static boost::thread* Create(Functor* f, void(Functor::*ptr)()) {
			boost::thread_group& group = GetGroup<groupId>();
			return group.create_thread(boost::bind(f, ptr));
		}

		template<typename Functor>
		static boost::thread* Create(Functor* f, void(Functor::*ptr)()) {
			return Create<ThreadType::MISC, Functor>(f, ptr);
		}

		template<ThreadType groupId, typename Function>
		static boost::thread* Create(Function f) {
			boost::thread_group& group = GetGroup<groupId>();
			return group.create_thread(f);
		}

		template<typename Function>
		static boost::thread* Create(Function f) {
			return Create<ThreadType::MISC, Function>(f);
		}

		template<ThreadType groupId>
		static boost::thread* Create(std::function<void()> f) {
			boost::thread_group& group = GetGroup<groupId>();
			return group.create_thread(f);
		}

		static boost::thread* Create(std::function<void()> f) {
			return Create<ThreadType::MISC>(f);
		}

		template<ThreadType id>
		static boost::thread_group& GetGroup() {
			assert(static_cast<uint32_t>(id) <= 3);
			static boost::thread_group group{};
			return group;
		}
	};
}