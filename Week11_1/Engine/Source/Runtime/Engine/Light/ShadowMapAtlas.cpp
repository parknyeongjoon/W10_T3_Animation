#include "ShadowMapAtlas.h"
#include <Engine/Engine.h>

FShadowMapAtlas::FShadowMapAtlas(ID3D11Device* Device, EAtlasType Type, int Resolution)
{
    if (Type == EAtlasType::SpotLight2D)
    {
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = Resolution;
        texDesc.Height = Resolution;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = Device->CreateTexture2D(&texDesc, nullptr, &AtlasTexture_2D);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to Create Atlas SpotLight Texture");
        }

        // SRV
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        hr = Device->CreateShaderResourceView(AtlasTexture_2D.Get(), &srvDesc, &AtlasSRV_2D);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to Create Atlas SpotLight SRV");
        }

        // DSV
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;
        dsvDesc.Flags = 0;
        hr = Device->CreateDepthStencilView(AtlasTexture_2D.Get(), &dsvDesc, &AtlasDSV_2D);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to Create Atlas SpotLight DSV");
        }

        // 아틀라스 슬롯 초기화
        const int SlotSize = Resolution / 4; // 슬롯 크기 (예: 1/4 크기)
        Slots2D.SetNum(16);
        for (int y = 0; y < 4; ++y)
        {
            for (int x = 0; x < 4; x++)
            {
                int index = y * 4 + x;
                Slots2D[index].Region = FIntRect(
                    x * SlotSize,
                    y * SlotSize,
                    SlotSize,
                    SlotSize);

                Slots2D[index].bInUse = false; // 슬롯 사용 여부 초기화
            }
        }

        // 기본 뷰포트
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<FLOAT>(Resolution);
        viewport.Height = static_cast<FLOAT>(Resolution);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
    }
    else if (Type == EAtlasType::PointLightCube)
    {
        // Cubemap 텍스쳐
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = Resolution;
        texDesc.Height = Resolution;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 6 * 16; // 6면 * 최대 16개
        texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = Device->CreateTexture2D(&texDesc, nullptr, &AtlasTexture_Cube);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to Create Atlas PointLightCube Texture");
        }

        // SRV
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
        srvDesc.TextureCubeArray.MostDetailedMip = 0;
        srvDesc.TextureCubeArray.MipLevels = 1;
        srvDesc.TextureCubeArray.First2DArrayFace = 0;
        srvDesc.TextureCubeArray.NumCubes = 16;

        hr = Device->CreateShaderResourceView(AtlasTexture_Cube.Get(), &srvDesc, &AtlasSRV_Cube);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to Create Atlas PointLightCube SRV");
        }


        for (int lightIdx = 0; lightIdx < 16; ++lightIdx) 
        {
            for (int face = 0; face < 6; ++face) 
            {
                D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
                dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
                dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                dsvDesc.Texture2DArray.MipSlice = 0;
                dsvDesc.Texture2DArray.FirstArraySlice = lightIdx * 6 + face; // (라이트 인덱스 × 6) + 면 인덱스
                dsvDesc.Texture2DArray.ArraySize = 1; // 한 번에 하나의 면만 렌더링

                // DSV 생성
                ComPtr<ID3D11DepthStencilView> dsv;
                HRESULT hr = Device->CreateDepthStencilView(
                    AtlasTexture_Cube.Get(),
                    &dsvDesc,
                    &dsv);

                if (FAILED(hr)) {
                    throw std::runtime_error("Failed to create CubeMap DSV");
                }

                AtlasDSV_Cube.Add(dsv);
            }
        }

    }
}

int FShadowMapAtlas::Allocate2DSlot(int RequestedSize)
{
    for (int i = 0; i < Slots2D.Num(); ++i)
    {
        if (Slots2D[i].bInUse == false && Slots2D[i].Region.Width >= RequestedSize)
        {
            Slots2D[i].bInUse = true;
            return i;
        }
    }

    // 슬롯을 찾지 못한 경우
    return -1;
}

void FShadowMapAtlas::Release2DSlot(int SlotIndex)
{
    if (SlotIndex >= 0 && SlotIndex < Slots2D.Num())
    {
        Slots2D[SlotIndex].bInUse = false;
    }
}

void FShadowMapAtlas::Clear2DSlots()
{
    for (auto& Slot : Slots2D)
    {
        Slot.bInUse = false;
    }

}

int FShadowMapAtlas::AllocateCubeSlot()
{
    // 사용 가능한 첫 번째 슬롯 반환 (Point Light는 전체 큐브맵 할당)
    for (int i = 0; i < 16; ++i)
    {
        if (!CubeSlots.Contains(i))
        {
            CubeSlots.Add(i);
            return i;
        }
    }

    return -1;
}

void FShadowMapAtlas::ReleaseCubeSlot(int SlotIndex)
{
    CubeSlots.Remove(SlotIndex);
}

void FShadowMapAtlas::ClearCubeSlots()
{
    CubeSlots.Empty();
}

ID3D11DepthStencilView* FShadowMapAtlas::GetDSVCube(int SlotId, int Face) const
{
    int idx = SlotId * 6 + Face;
    if (idx < 0 || idx >= AtlasDSV_Cube.Num())
    {
        return nullptr;
    }
    return AtlasDSV_Cube[idx].Get();
}

void FShadowMapAtlas::CreateVSMResource(ID3D11Device* Device, EAtlasType Type, int Resolution)
{
    if (Type == EAtlasType::SpotLight2D)
    {
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = Resolution;
        texDesc.Height = Resolution;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R32G32_FLOAT; // moment1, moment2 저장용
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = Device->CreateTexture2D(&texDesc, nullptr, &VSMAtlasTexture_2D);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create VSM Atlas Texture");
        }

        // SRV
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        ComPtr<ID3D11ShaderResourceView> AtlasSRV_VSM;
        hr = Device->CreateShaderResourceView(VSMAtlasTexture_2D.Get(), &srvDesc, &VSMAtlasSRV_2D);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create VSM Atlas SRV");
        }

        // RTV
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = texDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;

        hr = Device->CreateRenderTargetView(VSMAtlasTexture_2D.Get(), &rtvDesc, &VSMAtalsRTV_2D);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create VSM Atlas RTV");
        }

        // 기본 뷰포트
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<FLOAT>(Resolution);
        viewport.Height = static_cast<FLOAT>(Resolution);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
    }
}

size_t FShadowMapAtlas::GetEstimatedMemoryUsageInBytes(ELightType lightType)
{
    if (lightType == ELightType::PointLight)
    {
        // 포인트 라이트 (CubeMap Array)
        if (!AtlasTexture_Cube)
            return 0;

        D3D11_TEXTURE2D_DESC desc;
        AtlasTexture_Cube->GetDesc(&desc);

        // R32_TYPELESS 포맷: 픽셀당 4바이트
        const size_t bytesPerPixel = 4;

        // 전체 메모리 계산: 너비 × 높이 × 픽셀당 바이트 × 배열 크기(6면 × 16개)
        return static_cast<size_t>(desc.Width) * desc.Height * bytesPerPixel * desc.ArraySize;
    }
    else if (lightType == ELightType::SpotLight || lightType == ELightType::DirectionalLight)
    {
        // 스팟/방향성 라이트 (2D 아틀라스)
        if (!AtlasTexture_2D)
            return 0;

        D3D11_TEXTURE2D_DESC desc;
        AtlasTexture_2D->GetDesc(&desc);

        // R32_TYPELESS 포맷: 픽셀당 4바이트
        const size_t bytesPerPixel = 4;

        // 전체 메모리 계산: 너비 × 높이 × 픽셀당 바이트 × 배열 크기(1)
        return static_cast<size_t>(desc.Width) * desc.Height * bytesPerPixel * desc.ArraySize;
    }
}
