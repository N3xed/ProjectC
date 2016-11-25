#pragma once


namespace ProjectC {
	class IHandle {
	public:
		virtual ~IHandle() {}
	};
	
	template<typename T>
	class ILockHandle : public IHandle {
	public:
		typedef T& result_type;

		virtual bool IsLocked() const noexcept = 0;
		virtual void Unlock() const noexcept = 0;

		virtual T& Get() = 0;
	};
}