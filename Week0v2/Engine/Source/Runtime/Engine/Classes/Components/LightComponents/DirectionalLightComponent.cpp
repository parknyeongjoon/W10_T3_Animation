#include "DirectionalLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"
#include "EditorEngine.h"
#include "Math/JungleMath.h"

UDirectionalLightComponent::UDirectionalLightComponent()
{
    LightMap = new FTexture(nullptr, nullptr, 0,0,L"");
    
    FGraphicsDevice& Graphics = GEngine->graphicDevice;

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

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
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

UDirectionalLightComponent::UDirectionalLightComponent(const UDirectionalLightComponent& Other)
    : Super(Other)
    , Direction(Other.Direction)
{
}

//void UDirectionalLightComponent::SetDirection(FVector _newDir)
//{
//    //잘 안됨
//    FVector Axis = Direction.Cross(_newDir).Normalize();
//    float Angle = acosf(Direction.Normalize().Dot(_newDir.Normalize()));
//    GetOwner()->GetRootComponent()->SetRelativeQuat(FQuat::FromAxisAngle(Axis, Angle));
//    Direction = _newDir;
//}
const float SCENE_RADIUS = 3.0f;
FMatrix UDirectionalLightComponent::GetViewMatrix() const
{
    // 광원 위치 결정 (씬의 중심에서 반대 방향으로)
    FVector sceneCenter = FVector(0,0,0); // TODO: Scene Center 넣기
    FVector lightPos = sceneCenter - GetForwardVector() * SCENE_RADIUS;
    // 광원 뷰 행렬 계산
    FVector upVector = FVector(0.0f, 0.0f, 1.0f);
    if (abs(GetForwardVector().Dot(upVector) > 0.9f))
    {
        upVector = FVector(0.0f, 0.0f, 1.0f);
    }
    
    FMatrix lightView = JungleMath::CreateViewMatrix(
        lightPos,
        sceneCenter,
        upVector);

    return lightView;
}

FMatrix UDirectionalLightComponent::GetProjectionMatrix() const
{
    return JungleMath::CreateProjectionMatrix(
    45 * 2.0f,
    1.0f,
    0.1f,
    1000.0f
    );
    // 직교 투영 행렬 계산 (방향광은 직교 투영 사용)
    return JungleMath::CreateOrthoProjectionMatrix(1,1, 0.1f, SCENE_RADIUS);
}

UObject* UDirectionalLightComponent::Duplicate() const
{
    UDirectionalLightComponent* NewComp = FObjectFactory::ConstructObjectFrom<UDirectionalLightComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();

    return NewComp;
}

void UDirectionalLightComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    // 여기서 복사할 것? 방향뿐임
    UDirectionalLightComponent* SourceComp = Cast<UDirectionalLightComponent>(Source);
    SourceComp->Direction = Direction;
}

void UDirectionalLightComponent::PostDuplicate()
{
}

std::shared_ptr<FActorComponentInfo> UDirectionalLightComponent::GetActorComponentInfo()
{
    std::shared_ptr<FDirectionalLightComponentInfo> Info = std::make_shared<FDirectionalLightComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);
    Info->Direction = Direction;
    return Info;
}

void UDirectionalLightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FDirectionalLightComponentInfo& DirectionalLightInfo = static_cast<const FDirectionalLightComponentInfo&>(Info);
    Direction = DirectionalLightInfo.Direction;
}
