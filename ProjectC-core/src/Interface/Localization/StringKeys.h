#pragma once
#include <cstdint>

namespace ProjectC::Interface {
	const enum class StringKeys : size_t {
		//Status messages
		READY_MSG,
		INITIALIZING_MSG,
		SHUTTING_DOWN_MSG,
		LOADING_MSG,

		//Widget texts
		APP_TITLE,
		CONSOLE_TITLE,

		HOME_TEXT,
		OPTIONS_TEXT,
		INFO_TEXT,
		PEERS_TEXT,
		ADD_PEER_TEXT
	};
}