#pragma once

#include <d3d11.h>

class Texture
{
public:
    bool loadFromFile(ID3D11Device* g_pd3dDevice, const char* filename);

    const bool success() const { return _success; }
    const int w() const { return _width; }
    const int h() const { return _height; }
    ID3D11ShaderResourceView* img() const { return _texture; }


private:
    bool _success = false;
    int _width = 0;
    int _height = 0;
    ID3D11ShaderResourceView* _texture = nullptr;
};