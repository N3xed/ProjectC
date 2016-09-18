#pragma once
#pragma unmanaged

#include <stdint.h>
#include "../Interface/Window.h"

namespace ProjectC {
	namespace Networking {
		class Context {
		private:
			uint32_t m_id;
			Interface::Window& m_window;
			
		};
	}
}