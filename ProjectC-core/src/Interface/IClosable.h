#pragma once


namespace ProjectC::Interface {
	class IClosable {
	public:
		virtual void Close() = 0;
	};
}