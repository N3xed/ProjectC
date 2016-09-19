#pragma once
#pragma managed

namespace ProjectC {
	namespace Interface {

		template<typename Func, typename Arg, typename FnArg>
		public ref class DelegateWrapper {
		private:
			Func m_function;
			FnArg m_arg;
		public:
			DelegateWrapper(Func function, FnArg arg) : m_function(function), m_arg(arg)
			{}
			DelegateWrapper()
			{}

			void SetFunction(Func fn) {
				m_function = fn;
			}

			void SetArg(FnArg arg) {
				m_arg = arg;
			}

			void Call(Arg arg) {
				m_function(m_arg);
			}
		};
	}
}