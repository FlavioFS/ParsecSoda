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

void AppStyle::pushAlert()
{
	AppFonts::pushInput();
	AppColors::pushAlert();
}

void AppStyle::pushError()
{
	AppFonts::pushInput();
	AppColors::pushError();
}

void AppStyle::pop()
{
	AppFonts::pop();
	AppColors::pop();
}
