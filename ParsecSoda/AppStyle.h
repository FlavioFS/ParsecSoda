#pragma once

#include "AppFonts.h"
#include "AppColors.h"

class AppStyle
{
public:
	AppStyle(AppFonts& fonts, AppColors& colors);

	void pushTitle();
	void pushLabel();
	void pushInput();
	void pop();

private:
	AppFonts& _fonts;
	AppColors& _colors;
};

