#include "AppStyle.h"

AppStyle::AppStyle(AppFonts& fonts, AppColors& colors)
	: _fonts(fonts), _colors(colors)
{
}

void AppStyle::pushTitle()
{
	_fonts.pushTitle();
	_colors.pushTitle();
}

void AppStyle::pushLabel()
{
	_fonts.pushLabel();
	_colors.pushLabel();
}

void AppStyle::pushInput()
{
	_fonts.pushInput();
	_colors.pushInput();
}

void AppStyle::pop()
{
	_fonts.pop();
	_colors.pop();
}
