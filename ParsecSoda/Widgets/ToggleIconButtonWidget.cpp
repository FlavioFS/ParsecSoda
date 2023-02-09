#include "ToggleIconButtonWidget.h"

bool ToggleIconButtonWidget::render(Icon onIcon, Icon offIcon, bool isOn, const char* id, ImVec4 tintOn, ImVec4 tintOff, ImVec2 size)
{
    if (isOn)
    {
        return IconButton::render(onIcon, tintOn, size, id);
    }
    else
    {
        return IconButton::render(offIcon, tintOff, size, id);
    }

    return false;
}

bool ToggleIconButtonWidget::render(Icon onIcon, Icon offIcon, bool isOn, ImVec4 tintOn, ImVec4 tintOff, ImVec2 size)
{
    return ToggleIconButtonWidget::render(onIcon, offIcon, isOn, nullptr, tintOn, tintOff, size);
}

bool ToggleIconButtonWidget::render(Icon onIcon, Icon offIcon, bool isOn, const char* id, ImVec2 size)
{
    return ToggleIconButtonWidget::render(onIcon, offIcon, isOn, id, AppColors::positive, AppColors::negative, size);
}

bool ToggleIconButtonWidget::render(Icon onIcon, Icon offIcon, bool isOn, ImVec2 size)
{
    return ToggleIconButtonWidget::render(onIcon, offIcon, isOn, nullptr, AppColors::positive, AppColors::negative, size);
}
