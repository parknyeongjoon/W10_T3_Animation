#include "ShadowResource.h"
#include "ShadowMapAtlas.h"

FShadowResource::FShadowResource(ID3D11Device* Device, ELightType LightType, UINT ShadowResolution, bool bUseAtlas)
    :LightType(LightType)
{
    // 아틀라스를 사용하는 Resource의 경우에는 자원을 생성하지 않음
    if (bUseAtlas)
        return;

    switch (LightType)
    {
        case ELightType::DirectionalLight:
        case ELightType::SpotLight:
        {
            // Texture2D 생성
            D3D11_TEXTURE2D_DESC textureDesc = {};
            textureDesc.Width = ShadowResolution;
            textureDesc.Height = ShadowResolution;
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // 중요: TYPELESS
            textureDesc.MiscFlags = 0;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

            HRESULT hr = Device->CreateTexture2D(&textureDesc, nullptr, &ShadowTexture);
            if (FAILED(hr))
            {
                assert(TEXT("Shadow Texture creation failed"));
                return;
            }
            // SRV
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // 깊이 데이터만 읽기 위한 포맷
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;

            hr = Device->CreateShaderResourceView(ShadowTexture.Get(), &srvDesc, &ShadowSRV);
            if (FAILED(hr))
            {
                assert(TEXT("Shadow SRV creation failed"));
                return;
            }

            // DSV
            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = 0;

            ComPtr<ID3D11DepthStencilView> dsv;
            hr = Device->CreateDepthStencilView(ShadowTexture.Get(), &dsvDesc, &dsv);
            if (FAILED(hr))
            {
                assert(TEXT("Shadow DSV creation failed"));
                return;
            }
            ShadowDSVs.Add(dsv);

        // Viewport 설정
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<FLOAT>(ShadowResolution);
        viewport.Height = static_cast<FLOAT>(ShadowResolution);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        Viewports.Add(viewport);

        //VSM용 코드
        CreateVSMResources(Device, LightType, ShadowResolution);
        break;
    }
        case ELightType::PointLight:
        {
            // Texture2D - Cube Array
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = ShadowResolution;
            desc.Height = ShadowResolution;
            desc.MipLevels = 1;
            desc.ArraySize = 6;  // 큐브맵의 6개 면
            desc.Format = DXGI_FORMAT_R32_TYPELESS;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;  // 중요: 큐브맵으로 지정
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;

            HRESULT hr = Device->CreateTexture2D(&desc, nullptr, ShadowTexture.GetAddressOf());
            if (FAILED(hr))
            {
                assert(TEXT("Shadow Texture creation failed"));
                return;
            }

            // 모든 면을 한 번에 처리하는 DSV 생성
            D3D11_DEPTH_STENCIL_VIEW_DESC allFacesDSVDesc = {};
            allFacesDSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
            allFacesDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            allFacesDSVDesc.Texture2DArray.MipSlice = 0;
            allFacesDSVDesc.Texture2DArray.FirstArraySlice = 0;
            allFacesDSVDesc.Texture2DArray.ArraySize = 6;  // 모든 면 포함

            hr = Device->CreateDepthStencilView(ShadowTexture.Get(), &allFacesDSVDesc, &ShadowDSV);
            if (FAILED(hr))
            {
                assert(TEXT("Shadow All Faces DSV creation failed"));
                return;
            }

            // SRV
             // 3. 셰이더 리소스 뷰 생성
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = 1;
            srvDesc.TextureCube.MostDetailedMip = 0;

            hr = Device->CreateShaderResourceView(ShadowTexture.Get(), &srvDesc, &ShadowSRV);
            if (FAILED(hr))
            {
                assert(TEXT("Shadow SRV creation failed"));
                return;
            }

            // DSV 6 faces
            for (UINT face = 0; face < 6; ++face)
            {
                D3D11_DEPTH_STENCIL_VIEW_DESC faceDSVDesc = {};
                faceDSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
                faceDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                faceDSVDesc.Texture2DArray.MipSlice = 0;
                faceDSVDesc.Texture2DArray.FirstArraySlice = face;
                faceDSVDesc.Texture2DArray.ArraySize = 1;

                ComPtr<ID3D11DepthStencilView> dsv;
                hr = Device->CreateDepthStencilView(ShadowTexture.Get(), &faceDSVDesc, &dsv);
                if (FAILED(hr))
                {
                    assert(TEXT("Shadow DSV creation failed"));
                    return;
                }
                ShadowDSVs.Add(dsv);
                // Viewport 설정
                D3D11_VIEWPORT viewport = {};
                viewport.TopLeftX = 0;
                viewport.TopLeftY = 0;
                viewport.Width = static_cast<FLOAT>(ShadowResolution);
                viewport.Height = static_cast<FLOAT>(ShadowResolution);
                viewport.MinDepth = 0.0f;
                viewport.MaxDepth = 1.0f;
                Viewports.Add(viewport);
            }
            CreateVSMResources(Device, LightType, ShadowResolution);
            break;
        }
    }
}

FShadowResource::~FShadowResource()
{
    // remove this from the factory
    TArray<FShadowResource*>& ShadowResourcesArray = FShadowResourceFactory::ShadowResources[LightType];
    ShadowResourcesArray.RemoveSingle(this);
}

size_t FShadowResource::GetEsimatedMemoryUsageInBytes() const
{
    // 아틀라스가 있을 경우 아틀라스의 메모리 크기를 반환한다. 한 번만 호출함
    if (ParentAtlas)
        return ParentAtlas->GetEstimatedMemoryUsageInBytes(LightType);

    if (!ShadowTexture)
        return 0;

    D3D11_TEXTURE2D_DESC Desc;
    ShadowTexture->GetDesc(&Desc);

    size_t PixelSizeInBytes = 0;
    switch (Desc.Format)
    {
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        PixelSizeInBytes = 4;
        break;
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
        PixelSizeInBytes = 2;
        break;
    default:
        return 0;
    }

    size_t SizePerTex = static_cast<size_t>(Desc.Width) *
        static_cast<size_t>(Desc.Height) *
        PixelSizeInBytes;

    // 멀티샘플링 고려
    size_t SampleCount = FMath::Max((UINT)1, Desc.SampleDesc.Count);
    SizePerTex *= SampleCount;

    // 배열/밉맵 고려
    size_t Total = SizePerTex * Desc.ArraySize;
    if (Desc.MipLevels > 1)
    {
        // 밉맵 체인은 상위 레벨의 1/4씩 추가됨 (대략적인 추정)
        Total += Total / 3;
    }

    // 뷰 객체 메모리 추정 (대략 1KB per view)
    const size_t ViewOverhead = 1024;
    size_t ViewCount = 0;
    if (ShadowSRV) ViewCount++;
    ViewCount += ShadowDSVs.Num();
    Total += ViewCount * ViewOverhead;

    return Total;
}

ID3D11ShaderResourceView* FShadowResource::GetCubeAtlasSRVFace(ID3D11Device* Device, int cubeIndex, int faceIndex)
{
    if (!ParentAtlas || ParentAtlas->GetTextureCube() == nullptr)
        return nullptr;

    if (cubeIndex >= 16 || faceIndex >= 6 || !Device)
    {
        return nullptr;
    }

    ID3D11Texture2D* atlasTexture = ParentAtlas->GetTextureCube();

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.FirstArraySlice = cubeIndex * 6 + faceIndex;
    srvDesc.Texture2DArray.ArraySize = 1;

    ID3D11ShaderResourceView* srv = nullptr;
    HRESULT hr = Device->CreateShaderResourceView(atlasTexture, &srvDesc, &srv);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create cube atlas SRV");
    }

    return srv;
}

void FShadowResource::BindToAtlas(FShadowMapAtlas* Atlas, int SlotIndex)
{
    // 아틀라스에 바인딩
    ParentAtlas = Atlas;
    AtlasSlotIndex = SlotIndex;
}

void FShadowResource::UnbindFromAtlas()
{
    // 아틀라스 바인딩되어있는 상태라면 바인드해제
    if (ParentAtlas)
    {
        if (LightType == ELightType::PointLight)
        {
            ParentAtlas->ReleaseCubeSlot(AtlasSlotIndex);
        }
        else if (LightType == ELightType::SpotLight)
        {
            ParentAtlas->Release2DSlot(AtlasSlotIndex);
        }
        ParentAtlas = nullptr;
    }
}

void FShadowResource::CreateVSMResources(ID3D11Device* Device, ELightType LightType, UINT ShadowResolution)
{
    ComPtr<ID3D11RenderTargetView> rtv;
    D3D11_TEXTURE2D_DESC texDesc = {};
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    HRESULT hr;

    switch (LightType)
    {
    case ELightType::DirectionalLight:
    case ELightType::SpotLight:
        texDesc = {};
        texDesc.Width = static_cast<UINT>(ShadowResolution);              // shadow atlas 내 타일 크기
        texDesc.Height = static_cast<UINT>(ShadowResolution);
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = 0;

        hr = Device->CreateTexture2D(&texDesc, nullptr, &VSMTexture);
        if (FAILED(hr))
        {
            assert(TEXT("VSM Texture creation failed"));
            return;
        }

        rtvDesc = {};
        rtvDesc.Format = texDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;

        hr = Device->CreateRenderTargetView(VSMTexture.Get(), &rtvDesc, &rtv);
        if (FAILED(hr))
        {
            assert(TEXT("VSM RTV creation failed"));
            return;
        }
        VSMRTV.Add(rtv);

        srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        hr = Device->CreateShaderResourceView(VSMTexture.Get(), &srvDesc, &VSMSRV);
        if (FAILED(hr))
        {
            assert(TEXT("VSM SRV creation failed"));
            return;
        }
        break;
    case ELightType::PointLight:
        texDesc = {};
        texDesc.Width = static_cast<UINT>(ShadowResolution);
        texDesc.Height = static_cast<UINT>(ShadowResolution);
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 6;
        texDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        hr = Device->CreateTexture2D(&texDesc, nullptr, &VSMTexture);
        if (FAILED(hr))
        {
            assert(TEXT("VSM Texture Creation Failed"));
            return;
        }
        
        rtvDesc = {};
        for (int i = 0; i < 6; ++i)
        {
            rtvDesc = {};
            rtvDesc.Format = texDesc.Format;
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.MipSlice = 0;
            rtvDesc.Texture2DArray.FirstArraySlice = i;
            rtvDesc.Texture2DArray.ArraySize = 1;

            hr = Device->CreateRenderTargetView(VSMTexture.Get(), &rtvDesc, &rtv);
            if (FAILED(hr))
            {
                assert(TEXT("VSM RTV Creation Failed"));
                return;
            }
            VSMRTV.Add(rtv);
        }

        srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.ArraySize = texDesc.ArraySize;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.MipLevels = 1;

        hr = Device->CreateShaderResourceView(VSMTexture.Get(), &srvDesc, &VSMSRV);
        if (FAILED(hr))
        {
            assert(TEXT("VSM SRV Creation Failed"));
            return;
        }
    }
}

FShadowResource* FShadowResourceFactory::CreateShadowResource(ID3D11Device* Device, ELightType LightType, UINT ShadowResource, bool bUseAtlas)
{
    FShadowResource* shadowResource = new FShadowResource(Device, LightType, ShadowResource, bUseAtlas);
    if (shadowResource)
    {
        if (ShadowResources.Contains(shadowResource->LightType))
        {
            ShadowResources[shadowResource->LightType].Add(shadowResource);
        }
        else
        {
            ShadowResources.Add(shadowResource->LightType, TArray<FShadowResource*>());
            ShadowResources[shadowResource->LightType].Add(shadowResource);
        }
    }
    else
    {
        // Handle error
        assert(TEXT("Failed to create shadow resource"));
    }
    return shadowResource;
}
FShadowMemoryUsageInfo FShadowResourceFactory::GetShadowMemoryUsageInfo()
{
    FShadowMemoryUsageInfo memoryUsageInfo;
    for (const auto& pair : ShadowResources)
    {
        ELightType lightType = pair.Key;
        const TArray<FShadowResource*>& shadowResources = pair.Value;
        size_t totalMemory = 0;
        size_t count = 0;
        for (const FShadowResource* shadowResource : shadowResources)
        {
            if (lightType == ELightType::DirectionalLight)
            {
                totalMemory += shadowResource->GetEsimatedMemoryUsageInBytes();
            }
            /*else
            {
                totalMemory = shadowResource->GetEsimatedMemoryUsageInBytes();
            }*/
            count++;
        }
        memoryUsageInfo.TotalMemoryUsage += totalMemory;
        memoryUsageInfo.MemoryUsageByLightType.Add(lightType, totalMemory);
        memoryUsageInfo.LightCountByLightType.Add(lightType, count);
    }
    return memoryUsageInfo;
}

