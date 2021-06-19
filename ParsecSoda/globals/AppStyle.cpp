#include "AppStyle.h"

void AppStyle::pushTitle()
{
	AppFonts::pushTitle();
	AppColors::pushTitle();
}

void AppStyle::pushLabel()
{
	AppFonts::pushLabel();
	AppColors::pushLabel();
}

void AppStyle::pushInput()
{
	AppFonts::pushInput();
	AppColors::pushInput();
}

void AppStyle::pushPositive()
{
	AppFonts::pushInput();
	AppColors::pushPositive();
}

void AppStyle::pushNegative()
{
	AppFonts::pushInput();
	AppColors::pushNegative();
}

void AppStyle::pop()
{
	AppFonts::pop();
	AppColors::pop();
}
