#pragma once

#include <d3d11.h>

class Texture
{
public:
    bool loadFromFile(ID3D11Device* g_pd3dDevice, const char* filename);

    bool success = false;
    int width = 0;
    int height = 0;
    ID3D11ShaderResourceView* texture = nullptr;
};