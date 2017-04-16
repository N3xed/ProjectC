#pragma once

#include <functional>

namespace ProjectC {

	template<typename T>
	struct FunctionTraits;
	template<typename R, typename... Args>
	struct FunctionTraits<std::function<R(Args...)>> {
		static constexpr size_t ArgsCount = sizeof...(Args);
		typedef R ResultType;
		template<size_t index>
		struct Arg {
			typedef typename std::tuple_element<index, std::tuple<Args...>>::type Type;
		};
	};

	template<typename... Types>
	struct TypeList {
		template<size_t index>
		struct Get {
			typedef typename std::tuple_element<index, std::tuple<Types...>>::type Type;
		};
	};

	template<typename Callable>
	class InvocationList {};

	/// <summary>
	/// A list that can chain event listeners.
	/// </summary>
	template<typename R, typename... Args>
	class InvocationList<R(Args...)> {
	public:
		typedef std::function<void(Args...)> FunctionType;

	private:
		struct Node {
			FunctionType Function;
			Node* Next;
			Node() : Function(), Next(nullptr) {	}
			~Node() {
				delete Next;
			}
			void operator()(Args... args) {
				if(Function)
					Function(std::forward<Args>(args)...);

				if(Next != nullptr)
					(*Next)(std::forward<Args>(args)...);
			}
		};
		Node* m_front;
		Node* m_back;
	public:

		InvocationList() : m_front(new Node()), m_back(m_front)
		{}
		~InvocationList() {
			delete m_front;
		}

		void operator()(Args... args) {
			(*m_front)(std::forward<Args>(args)...);
		}

		void Add(FunctionType func) {
			m_back->Function = func;
			m_back->Next = new Node();
			m_back = m_back->Next;
		}

		/// <summary>
		/// Removes the specified function.
		/// </summary>
		/// <param name="func">The function.</param>
		/// <returns><c>true</c> if the function was removed, <c>false</c> if it wasn't.</returns>
		bool Remove(FunctionType func) {
			for (Node* last = m_front, iter = m_front; iter != nullptr; iter = iter->Next) {
				if (iter->Function.target_type() == func.target_type()) {
					last = iter->Next;
					iter.Next = nullptr;
					delete iter;
					return true;
				}
				last = iter;
			}
		}
		void Clear() {
			delete m_front;
			m_front = new Node();
			m_back = m_front;
		}
	};

}