#pragma once

#include "IHandle.h"

namespace ProjectC {
	class IDisposable : public IHandle {
	public:
		virtual ~IDisposable() {};

		virtual void Dispose() = 0;
	};
}