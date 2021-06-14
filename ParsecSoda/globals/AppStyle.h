#pragma once

#include "AppFonts.h"
#include "AppColors.h"

class AppStyle
{
public:
	static void pushTitle();
	static void pushLabel();
	static void pushInput();
	static void pushAlert();
	static void pushError();
	static void pop();
};

