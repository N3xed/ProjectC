#pragma once
#pragma managed

namespace ProjectC {
	namespace Interface {

		template<typename Function, typename DelegateArgument, typename FunctionArgument>
		public ref class DelegateWrapper {
		private:
			Function m_function;
			FunctionArgument m_arg;
		public:
			DelegateWrapper(Function function, FunctionArgument arg) : m_function(function), m_arg(arg)
			{}
			DelegateWrapper()
			{}

			void SetFunction(Function fn) {
				m_function = fn;
			}

			void SetArg(FunctionArgument arg) {
				m_arg = arg;
			}

			void Call(DelegateArgument arg) {
				m_function(m_arg);
			}
		};
	}
}