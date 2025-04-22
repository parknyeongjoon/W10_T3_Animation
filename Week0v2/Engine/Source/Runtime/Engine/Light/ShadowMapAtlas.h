#pragma once
#include "Define.h"
#include <wrl/client.h> // Add this include for Microsoft::WRL::ComPtr

using Microsoft::WRL::ComPtr; // Ensure ComPtr is properly defined

class FShadowMapAtlas
{
public:
    enum class EAtlasType
    {
        None,
        SpotLight2D,
        PointLightCube,
    };

private:
    struct FAtlasSlot
    {
        bool bInUse = false;
        FIntRect Region; // x, y, width, height
    };
   
private:
    EAtlasType AtlasType = EAtlasType::None;

    ComPtr<ID3D11Texture2D> AtlasTexture_2D;
    ComPtr<ID3D11ShaderResourceView> AtlasSRV_2D;

    // Spotlight
    ComPtr<ID3D11DepthStencilView> AtlasDSV_2D;
    TArray<FAtlasSlot> AtlasSlots_2D;

    // PointLight
    ComPtr<ID3D11Texture2D> AtlasTexture_Cube;
    ComPtr<ID3D11ShaderResourceView> AtlasSRV_Cube;
    ComPtr<ID3D11DepthStencilView> AtlasDSV_Cube[6];

    TArray<int> CubeSlots;

public:
    FShadowMapAtlas(ID3D11Device* Device, EAtlasType Type, int Resolution = 4096);

    int Allocate2DSlot(int RequestedSize);
    void Release2DSlot(int SlotIndex);

    ID3D11DepthStencilView* GetDSV2D() const { return AtlasDSV_2D.Get(); }

    int AllocateCubeSlot(int RequestedSize);
    void ReleaseCubeSlot(int SlotIndex);
    ID3D11DepthStencilView* GetDSVCube(int Face) const;
    ID3D11ShaderResourceView* GetSRVCube() const { return AtlasSRV_Cube.Get(); }

    EAtlasType GetType() const { return AtlasType; }
};