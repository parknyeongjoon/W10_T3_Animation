#include "SpotLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"
#include "Math/JungleMath.h"
#include <D3D11RHI/GraphicDevice.h>
#include "EditorEngine.h"

USpotLightComponent::USpotLightComponent()
    : Super()
{
    LightMap = new FTexture(nullptr, nullptr, 0,0,L"");
    ShadowMap = new FTexture(nullptr, nullptr, 0,0,L"");
    
    FGraphicsDevice& Graphics = GEngine->graphicDevice;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = 1024;
    desc.Height = 1024;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32_TYPELESS;;     // 중요: TYPELESS
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;

    Graphics.Device->CreateTexture2D(&desc, nullptr, &ShadowMap->Texture);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    Graphics.Device->CreateDepthStencilView(ShadowMap->Texture, &dsvDesc, &DSV);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // 깊이 데이터만 읽기 위한 포맷
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    Graphics.Device->CreateShaderResourceView(ShadowMap->Texture, &srvDesc, &ShadowMap->TextureSRV);

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = 1024;
    textureDesc.Height = 1024;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = Graphics.Device->CreateTexture2D(&textureDesc, nullptr, &LightMap->Texture);
    if (FAILED(hr))
    {
        assert(TEXT("SceneColorBuffer creation failed"));
        return;
    }

    D3D11_RENDER_TARGET_VIEW_DESC SceneColorRTVDesc = {};
    SceneColorRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      // 색상 포맷
    SceneColorRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

    hr = Graphics.Device->CreateRenderTargetView(LightMap->Texture, &SceneColorRTVDesc, &LightRTV);
    if (FAILED(hr))
    {
        assert(TEXT("SceneColorBuffer creation failed"));
        return;
    }

    srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = Graphics.Device->CreateShaderResourceView(LightMap->Texture, &srvDesc, &LightMap->TextureSRV);
    if (FAILED(hr))
    {
        assert(TEXT("SceneColorBuffer creation failed"));
        return;
    }
}

USpotLightComponent::USpotLightComponent(const USpotLightComponent& Other)
    : Super(Other)
    , InnerConeAngle(Other.InnerConeAngle)
    , OuterConeAngle(Other.OuterConeAngle)
{

}

USpotLightComponent::~USpotLightComponent()
{
    // release dx11 resources
    if (DSVBuffer)
    {
        DSVBuffer->Release();
        DSVBuffer = nullptr;
    }
    if (DSV)
    {
        DSV->Release();
        DSV = nullptr;
    }
    if (ShadowMap)
    {
        ShadowMap->Release();
        ShadowMap = nullptr;
    }
    if (RTVBuffer)
    {
        RTVBuffer->Release();
        RTVBuffer = nullptr;
    }
    if (LightRTV)
    {
        LightRTV->Release();
        LightRTV = nullptr;
    }
    if (RTVSRV)
    {
        RTVSRV->Release();
        RTVSRV = nullptr;
    }

}

void USpotLightComponent::SetInnerConeAngle(float Angle)
{
    Angle = JungleMath::DegToRad(Angle);
    InnerConeAngle = Angle;
}

void USpotLightComponent::SetOuterConeAngle(float Angle)
{
    Angle = JungleMath::DegToRad(Angle);
    OuterConeAngle = Angle;
}

UObject* USpotLightComponent::Duplicate() const
{
    USpotLightComponent* NewComp = FObjectFactory::ConstructObjectFrom<USpotLightComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();

    return NewComp;
}

void USpotLightComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void USpotLightComponent::PostDuplicate()
{
}

FMatrix USpotLightComponent::GetViewMatrix()
{
    FVector Up = FVector{ 0.0f, 0.0f, 1.0f };
    FVector Forward = GetForwardVector();
    float dot = abs(Up.Dot(Forward));
    if (dot > 0.99f)
        Up = FVector(1.0f, 0.0f, 0.0f);
    return JungleMath::CreateViewMatrix(GetComponentLocation(),
        Forward + GetComponentLocation(),
        Up);
} 

FMatrix USpotLightComponent::GetProjectionMatrix() const
{
    return JungleMath::CreateProjectionMatrix(
        OuterConeAngle * 2.0f,
        1.0f,
        0.1f,
        1000.0f
    );
}

std::shared_ptr<FActorComponentInfo> USpotLightComponent::GetActorComponentInfo()
{
    std::shared_ptr<FSpotlightComponentInfo> Info = std::make_shared<FSpotlightComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);

    Info->InnerConeAngle = InnerConeAngle;
    Info->OuterConeAngle = OuterConeAngle;

    return Info;
}

void USpotLightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FSpotlightComponentInfo& PointLightInfo = static_cast<const FSpotlightComponentInfo&>(Info);
    InnerConeAngle = PointLightInfo.InnerConeAngle;
    OuterConeAngle = PointLightInfo.OuterConeAngle;
}