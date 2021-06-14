#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

#define APPFONTS_EXTRA_LIGHT	"fonts/Montserrat-ExtraLight.ttf"
#define APPFONTS_LIGHT			"fonts/Montserrat-Light.ttf"
#define APPFONTS_REGULAR		"fonts/Montserrat-Regular.ttf"
#define APPFONTS_MEDIUM			"fonts/Montserrat-Medium.ttf"
#define APPFONTS_SEMI_BOLD		"fonts/Montserrat-SemiBold.ttf"
#define APPFONTS_BOLD			"fonts/Montserrat-Bold.ttf"
#define APPFONTS_EXTRA_BOLD		"fonts/Montserrat-ExtraBold.ttf"

class AppFonts
{

public:
	static void init(ImGuiIO& io);

	static void pushLabel();
	static void pushTitle();
	static void pushInput();
	static void pop();

	static ImFont* label;
	static ImFont* title;
	static ImFont* input;
};

