#pragma once
#include <memory>
#include "Texture.h"
#include "Container/Map.h"

class FGraphicsDevice;
class FResourceManager
{

public:
    void Initialize(FGraphicsDevice* device);
    void Release();
    HRESULT LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename);
    HRESULT LoadTextureFromDDS(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename);

    UTexture* GetTexture(const FWString& name);
    UTexture* GetDefaultWhiteTexture();
private:
    FGraphicsDevice* GraphicDevice = nullptr;
    TMap<FWString, UTexture*> Textures;
};