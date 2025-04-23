#pragma once  
#include "Define.h"  
#include "Container/Map.h"
#include <wrl/client.h> 

using Microsoft::WRL::ComPtr;

class FShadowMapAtlas; 

struct FShadowResource
{
    //static ID3D11SamplerState* comparisonSampler;
    ComPtr<ID3D11ShaderResourceView> ShadowSRV; // Example usage of ComPtr  
    TArray<ComPtr<ID3D11DepthStencilView>> ShadowDSVs;
    ComPtr<ID3D11Texture2D> ShadowTexture;
    TArray<D3D11_VIEWPORT> Viewports;
    ComPtr<ID3D11DepthStencilView>ShadowDSV;   
    ELightType GetLightType() const { return LightType; }

    //VSM용
    ComPtr<ID3D11Texture2D> VSMTexture;
    TArray< ComPtr<ID3D11RenderTargetView>> VSMRTV;
    ComPtr<ID3D11ShaderResourceView> VSMSRV;

    // Face의 개수. Directional/Spot Light는 1개, Point Light는 6개..  
    int NumFaces = 1;

    ELightType LightType;

    FShadowResource() = default;
    FShadowResource(ID3D11Device* Device, ELightType LightType, UINT ShadowResolution, bool bUseAtlas);
    ~FShadowResource();

    size_t GetEsimatedMemoryUsageInBytes() const;
    ID3D11ShaderResourceView* GetSRV() const { return ShadowSRV.Get(); }
    ID3D11Texture2D* GetTexture() const { return ShadowTexture.Get(); }
    ID3D11DepthStencilView* GetDSV(int faceIndex = 0) const
    {
        if (faceIndex < 0 || faceIndex >= ShadowDSVs.Num())
            return nullptr;
        return ShadowDSVs[faceIndex].Get();
    }
    D3D11_VIEWPORT GetViewport(int faceIndex = 0) const
    {
        if (faceIndex < 0 || faceIndex >= Viewports.Num())
            return {};
        return Viewports[faceIndex];
    }

    void CreateVSMResources(ID3D11Device* Device, ELightType LightType, UINT ShadowResolution);
    ID3D11RenderTargetView* GetVSMRTV(int face = 0) const { return VSMRTV[face].Get(); }
    ID3D11ShaderResourceView* GetVSMSRV() const { return VSMSRV.Get(); }

    ID3D11ShaderResourceView* GetCubeFaceSRV(
        ID3D11Device* device,
        ID3D11Texture2D* cubeTexture,
        UINT faceIndex)  // 0 ~ 5
    {
        if (faceIndex >= 6 || faceIndex<0) {
            assert(!"Invalid cube face index (0~5 only)");
            return nullptr;
        }

        // SRV 설정
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // 원본이 R32_TYPELESS일 때 대응
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.FirstArraySlice = faceIndex;
        srvDesc.Texture2DArray.ArraySize = 1;

        ID3D11ShaderResourceView* faceSRV = nullptr;
        HRESULT hr = device->CreateShaderResourceView(cubeTexture, &srvDesc, &faceSRV);
        if (FAILED(hr)) {
            assert(!"Failed to create cube face SRV");
            return nullptr;
        }

        return faceSRV;
    }

    ID3D11ShaderResourceView* GetCubeAtlasSRVFace(ID3D11Device* Device, int cubeIndex, int faceIndex);
    
private:
    FShadowMapAtlas* ParentAtlas = nullptr;
    int AtlasSlotIndex = -1;

public:
    bool IsInAtlas() const { return ParentAtlas != nullptr; }
    int GetAtlasSlotIndex() const { return AtlasSlotIndex; }
    void BindToAtlas(FShadowMapAtlas* Atlas, int SlotIndex);
    void UnbindFromAtlas();
    FShadowMapAtlas* GetParentAtlas() const { return ParentAtlas; }
};

struct FShadowMemoryUsageInfo
{
    size_t TotalMemoryUsage = 0;
    TMap<ELightType, size_t> MemoryUsageByLightType;
    TMap<ELightType, size_t> LightCountByLightType;
};

class FShadowResourceFactory
{
public:
    static inline TMap<ELightType, TArray<FShadowResource*>> ShadowResources;
    static FShadowResource* CreateShadowResource(ID3D11Device* Device, ELightType LightType, UINT ShadowResource, bool bUseAtlas = true);
    static FShadowMemoryUsageInfo GetShadowMemoryUsageInfo();
};
