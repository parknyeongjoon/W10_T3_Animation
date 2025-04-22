#include "ShadowMapAtlas.h"

FShadowMapAtlas::FShadowMapAtlas(ID3D11Device* Device, EAtlasType Type, int Resolution)
{
    if (Type == EAtlasType::SpotLight2D)
    {

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
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = Device->CreateTexture2D(&texDesc, nullptr, &AtlasTexture_Cube);

    }
}
