#pragma once

#ifndef CONFIGKEYS_H
#define CONFIGKEYS_H

#include <boost/preprocessor.hpp>
#include <boost/preprocessor/slot/counter.hpp>

#define BOOST_PP_COUNTER 0
#define DECLARE_GETTER_STRUCT template<size_t index> struct Key
#define DEFINE_KEY(name, value, index) static constexpr const char* name = value;\
										template<> struct Key<index>{ static constexpr const char* Value = name; };
#define _GET(z, n, a) case n: return Key<n>::Value;
#define DEFINE_GETTER_FUNC(size) static const char* Get(size_t i){switch(i){ BOOST_PP_REPEAT(size, _GET, ~) default: assert(false); return nullptr; }}\
									static constexpr const size_t Size = size
namespace ProjectC {
	struct ConfigKeys {
		DECLARE_GETTER_STRUCT;
		// style
		DEFINE_KEY(INTERFACE_STYLE_COL, "interface.style.backgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY(INTERFACE_STYLE_FONTCOL, "interface.style.fontColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()

		// style.home
		DEFINE_KEY( INTERFACE_STYLE_HOME_BUTTONCOL, "interface.style.home.buttonBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_HOME_BUTTONACTIVECOL, "interface.style.home.buttonActiveBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_HOME_BUTTONHOVERCOL, "interface.style.home.buttonHoverBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_HOME_BUTTONFONTCOL, "interface.style.home.buttonFontColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_HOME_FONTCOL, "interface.style.home.fontColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_HOME_BORDERCOL, "interface.style.home.borderColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()

		// style.nav
		DEFINE_KEY( INTERFACE_STYLE_NAV_FONTCOL, "interface.style.nav.textColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_FONTDISABLEDCOL, "interface.style.nav.textDisabledColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_COL, "interface.style.nav.backgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_BUTTONCOL, "interface.style.nav.buttonBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_BUTTONACTIVECOL, "interface.style.nav.buttonActiveBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_BUTTONHOVERCOL, "interface.style.nav.buttonHoverBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_BUTTONDISABLEDCOL, "interface.style.nav.buttonDisabledBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_BUTTONDISABLEDACTIVECOL, "interface.style.nav.buttonDisabledActiveBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_BUTTONDISABLEDHOVERCOL, "interface.style.nav.buttonDisabledHoverBackgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_NAV_ARROWCOL, "interface.style.nav.arrowColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()

		// style.statusBar
		DEFINE_KEY( INTERFACE_STYLE_STATUSBAR_BACKGROUNDCOL, "interface.style.statusBar.backgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_STATUSBAR_FONTCOL, "interface.style.statusBar.fontColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()

		// style.console
		DEFINE_KEY( INTERFACE_STYLE_CONSOLE_BACKGROUNDCOL, "interface.style.console.backgroundColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_CONSOLE_FONTNORMALCOL, "interface.style.console.fontNormalColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_CONSOLE_FONTWARNCOL, "interface.style.console.fontWarnColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()
		DEFINE_KEY( INTERFACE_STYLE_CONSOLE_FONTFATALCOL, "interface.style.console.fontFatalColour", BOOST_PP_COUNTER);
#include BOOST_PP_UPDATE_COUNTER()

		DEFINE_GETTER_FUNC(BOOST_PP_COUNTER);
	};
}

#undef DEFINE_KEY
#undef DECLARE_GETTER_STRUCT
#undef DEFINE_GETTER_FUNC
#undef _GET
#endif