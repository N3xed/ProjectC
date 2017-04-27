#pragma once
#include <cstdint>


namespace ProjectC::Interface::Detail {
	static const char ProcessMessageName[] = "ProjC";

	enum class BrowserProcessMessageType : int32_t {
		LOG = 0,
		ON_EXECUTE = 1,
		EXEC_MODULE_LISTENER = 2,
		GET_STR_RESOURCE = 3,
		QUERY_RESPONSE = 4,
		GET_WINDOW_TITLE = 5,
		SET_WINDOW_TITLE = 6,
		SHOW_WINDOW = 7,
		CLOSE_WINDOW = 8,
		OPEN_WINDOW = 9
	};

	enum class RenderProcessMessageType : int32_t {
		STR_RESOURCE_RESPONSE = 0,
		MODULE_CHANGED = 1,
		EXEC_JS_LISTENER = 2,
		STATUSBAR_PUSH_STATUS = 3,
		STATUSBAR_ADD_CHILD = 4,
		STATUSBAR_REMOVE_ALL_CHILDS = 5,
		STATUSBAR_REMOVE_CHILD_BY_ID = 6,
		STATUSBAR_REMOVE_CHILDS_BY_CLASS = 7,
		STATUSBAR_REMOVE_CHILDS_BY_ATTR = 8,
		SHOW = 9,
		QUERY = 10,
		EXEC_JS_CODE = 11,
		WINDOW_TITLE_RESPONSE = 12
	};
}