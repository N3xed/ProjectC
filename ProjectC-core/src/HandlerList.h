#pragma once

namespace ProjectC {
	template<typename Callable>
	class HandlerList {};

	/// <summary>
	/// A list that chain-invokes event handlers.
	/// Return <c>true</c> from the handler to indicate, it was handled, <c>false</c> if not.
	/// </summary>
	template<typename R, typename... Args>
	class HandlerList<R(Args...)> {
	public:
		typedef std::function<bool(Args...)> FunctionType;

	private:
		struct Node {
			FunctionType Function;
			Node* Next;
			Node() : Function(nullptr), Next(nullptr) {	}
			~Node() {
				delete Next;
			}
			bool operator()(Args... args) {
				if (Function)
					if (Function(std::forward<Args>(args)...))
						return true;

				if (Next != nullptr)
					return (*Next)(std::forward<Args>(args)...);
				return false;
			}
		};
		Node* m_front;
		Node* m_back;
	public:

		HandlerList() : m_front(new Node()), m_back(m_front)
		{}
		~HandlerList() {
			delete m_front;
		}

		bool operator()(Args... args) {
			return (*m_front)(std::forward<Args>(args)...);
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
	};
}