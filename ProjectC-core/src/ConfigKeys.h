#pragma once

#define DEFINE_KEY(name, value)	static constexpr const char* name = value
#define CONFIG_KEY(name) ProjectC::ConfigKeys::name

namespace ProjectC {
	struct ConfigKeys {
		// fontStyle
		DEFINE_KEY(FONTSTYLE, "font");

		DEFINE_KEY(FONTSTYLE_NAME, "name");
		DEFINE_KEY(FONTSTYLE_SIZE, "size");
		DEFINE_KEY(FONTSTYLE_MODE, "mode");
		DEFINE_KEY(FONTSTYLE_WEIGHT, "weight");

		//styles
		DEFINE_KEY(NAVBUTTONSTYLE, "navButton");
		DEFINE_KEY(HOMEBUTTONSTYLE, "homeButton");
		DEFINE_KEY(OPTIONSBUTTONSTYLE, "optionsButton");
		DEFINE_KEY(CLOSEBUTTONSTYLE, "closeButton");

		DEFINE_KEY(NAVSTYLE, "nav");
		DEFINE_KEY(NAVARROWSTYLE, "navArrowColour");
		DEFINE_KEY(HOMESTYLE, "home");
		DEFINE_KEY(FRAMESTYLE, "frame");
		DEFINE_KEY(STATUSBARSTYLE, "statusbar");
		DEFINE_KEY(CONSOLESTYLE, "console");

		DEFINE_KEY(LOADERSTYLE, "loader");
		DEFINE_KEY(DISPLAYERSTYLE, "displayer");

		//consoleStyle
		DEFINE_KEY(CONSOLESTYLE_FONTWARNCOL, "fontWarnColour");
		DEFINE_KEY(CONSOLESTYLE_FONTFATALCOL, "fontFatalColour");

		//style
		DEFINE_KEY(STYLE_FGCOL, "foregroundColour");
		DEFINE_KEY(STYLE_BGCOL, "backgroundColour");
		DEFINE_KEY(STYLE_BGHOVERCOL, "backgroundHoverColour");
		DEFINE_KEY(STYLE_BGACTIVECOL, "backgroundActiveColour");
		DEFINE_KEY(STYLE_BGDISABLEDCOL, "backgroundDisabledColour");
		DEFINE_KEY(STYLE_BORDERCOL, "borderColour");
		DEFINE_KEY(STYLE_BORDERWIDTH, "borderWidth");
		DEFINE_KEY(STYLE_TYPENAME, "typename");
		DEFINE_KEY(STYLE_FONTCOL, "fontColour");
		DEFINE_KEY(STYLE_FONTDISABLEDCOL, "fontDisabledColour");
		DEFINE_KEY(STYLE_FONTFOCUSCOL, "fontFocusColour");
		DEFINE_KEY(STYLE_BGFOCUSCOL, "backgroundFocusColour");

		DEFINE_KEY(HOMESTYLE_INFOBGCOL, "infoBackgroundColour");
		DEFINE_KEY(HOMESTYLE_INFOFONTCOL, "infoFontColour");
		DEFINE_KEY(HOMESTYLE_INFOBORDERCOL, "infoBorderColour");
		DEFINE_KEY(HOMESTYLE_MODULEBGCOL, "moduleBackgroundColour");
		DEFINE_KEY(HOMESTYLE_MODULEFONTCOL, "moduleFontColour");
		DEFINE_KEY(HOMESTYLE_MODULEBORDERCOL, "moduleBorderColour");
		DEFINE_KEY(HOMESTYLE_BUTTONSTYLE, "button");

		DEFINE_KEY(STYLE, "interface.style");

		//interface
		DEFINE_KEY(INTERFACE, "interface");

		DEFINE_KEY(INTERFACE_LASTSIZE, "lastSize");
		DEFINE_KEY(INTERFACE_LASTSIZESTATE, "lastSizeState");
		DEFINE_KEY(INTERFACE_LASTPOS, "lastPosition");
		DEFINE_KEY(INTERFACE_LASTMODULE, "lastApplication");
	};
}