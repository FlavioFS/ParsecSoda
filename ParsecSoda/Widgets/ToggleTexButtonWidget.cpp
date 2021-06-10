#include "ToggleTexButtonWidget.h"

ToggleTexButtonWidget::ToggleTexButtonWidget()
    : _onTex(nullptr), _offTex(nullptr), _isOn(false), _size(TOGGLETEXBUTTON_SIZE)
{
}

ToggleTexButtonWidget::ToggleTexButtonWidget(ID3D11ShaderResourceView* onTex, ID3D11ShaderResourceView* offTex, bool isOn, ImVec2 size)
{
    _onTex = onTex;
    _offTex = offTex;
    setOn(isOn);
    _size = size;
}

bool ToggleTexButtonWidget::render(bool& isEnabled)
{
    if (render())
    {
        _isOn = !_isOn;
        isEnabled = _isOn;
        return true;
    }

    return false;
}

bool ToggleTexButtonWidget::render()
{
    if (_isOn)
    {
        if (ImGui::ImageButton((void*)_onTex, _size))
        {
            return true;
        }
    }
    else
    {
        if (ImGui::ImageButton((void*)_offTex, _size))
        {
            return true;
        }
    }

    return false;
}

void ToggleTexButtonWidget::setOn(bool isOn)
{
    _isOn = isOn;
}
