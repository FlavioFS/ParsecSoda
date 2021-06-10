#pragma once
#include "IWidget.h"

#include "../imgui/imgui.h"
#include <d3d11.h>

#define TOGGLETEXBUTTON_SIZE ImVec2(40,40)

class ToggleTexButtonWidget
{
public:
	ToggleTexButtonWidget();
	ToggleTexButtonWidget(ID3D11ShaderResourceView* onTex, ID3D11ShaderResourceView* offTex, bool isOn, ImVec2 size = TOGGLETEXBUTTON_SIZE);
	bool render(bool &isEnabled);
	bool render();
	void setOn(bool isOn);
	
private:
	ImVec2 _size;
	ID3D11ShaderResourceView* _onTex;
	ID3D11ShaderResourceView* _offTex;
	bool _isOn = false;
};