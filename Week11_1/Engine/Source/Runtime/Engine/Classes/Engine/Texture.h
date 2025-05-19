#pragma once
#include "D3D11RHI/GraphicDevice.h"
#include "container/String.h"
#include "HAL/PlatformType.h"
struct FTexture
{
    FTexture(ID3D11ShaderResourceView* SRV, ID3D11Texture2D* Texture2D, uint32 _width, uint32 _height, FWString _path)
        : TextureSRV(SRV), Texture(Texture2D), width(_width), height(_height), path(_path)
    {}
    ~FTexture()
    {
		
    }
    void Release()
    {
        if (TextureSRV) { TextureSRV->Release(); TextureSRV = nullptr; }
        if (Texture) { Texture->Release(); Texture = nullptr; }
    }
    
    ID3D11ShaderResourceView* TextureSRV = nullptr;
    ID3D11Texture2D* Texture = nullptr;
    FWString path;
    uint32 width;
    uint32 height;
};
