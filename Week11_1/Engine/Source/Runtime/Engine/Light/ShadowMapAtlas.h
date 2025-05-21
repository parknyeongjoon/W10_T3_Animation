#pragma once
#include "Define.h"
#include <wrl/client.h> // Add this include for Microsoft::WRL::ComPtr

using Microsoft::WRL::ComPtr; // Ensure ComPtr is properly defined

#define SHADOW_ATLAS_SIZE 4096
enum class EAtlasType
{
    None,
    SpotLight2D,
    PointLightCube,
};
class FShadowMapAtlas
{

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
    TArray<FAtlasSlot> Slots2D;

    //VSM
    ComPtr<ID3D11Texture2D> VSMAtlasTexture_2D = nullptr;
    ComPtr<ID3D11ShaderResourceView> VSMAtlasSRV_2D = nullptr;
    ComPtr<ID3D11RenderTargetView> VSMAtalsRTV_2D = nullptr;

    // PointLight
    ComPtr<ID3D11Texture2D> AtlasTexture_Cube;
    ComPtr<ID3D11ShaderResourceView> AtlasSRV_Cube;
    TArray<ComPtr<ID3D11DepthStencilView>> AtlasDSV_Cube;

    TArray<int> CubeSlots;

public:
    FShadowMapAtlas(ID3D11Device* Device, EAtlasType Type, int Resolution = SHADOW_ATLAS_SIZE);

    ID3D11Texture2D* GetTexture2D() const { return AtlasTexture_2D.Get(); }
    ID3D11Texture2D* GetTextureCube() const { return AtlasTexture_Cube.Get(); }


    int Allocate2DSlot(int RequestedSize);
    void Release2DSlot(int SlotIndex);
    void Clear2DSlots();

    ID3D11DepthStencilView* GetDSV2D() const { return AtlasDSV_2D.Get(); }
    ID3D11ShaderResourceView* GetSRV2D() const { return AtlasSRV_2D.Get(); }

    int AllocateCubeSlot();
    void ReleaseCubeSlot(int SlotIndex);
    void ClearCubeSlots();

    ID3D11DepthStencilView* GetDSVCube(int SlotID, int Face) const;
    ID3D11ShaderResourceView* GetSRVCube() const { return AtlasSRV_Cube.Get(); }

    //VSM
    void CreateVSMResource(ID3D11Device* Device, EAtlasType Type, int Resolution = SHADOW_ATLAS_SIZE);
    ID3D11Texture2D* GetVSMTexture2D() { return VSMAtlasTexture_2D.Get(); }
    ID3D11ShaderResourceView* GetVSMSRV2D() { return VSMAtlasSRV_2D.Get(); }
    ID3D11RenderTargetView* GetVSMRTV2D() { return VSMAtalsRTV_2D.Get(); }

    EAtlasType GetType() const { return AtlasType; }

    size_t GetEstimatedMemoryUsageInBytes(ELightType lightType);
};