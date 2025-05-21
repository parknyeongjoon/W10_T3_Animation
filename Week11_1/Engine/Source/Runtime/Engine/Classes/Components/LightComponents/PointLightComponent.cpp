#include "PointLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"
#include <Math/JungleMath.h>
#include "EditorEngine.h"
#include "LaunchEngineLoop.h"

UPointLightComponent::UPointLightComponent()
{
    ShadowResource = FShadowResourceFactory::CreateShadowResource(GEngineLoop.GraphicDevice.Device, ELightType::PointLight, 1024);
    Radius = 25.f;
    Intensity = 5.f;
}

UObject* UPointLightComponent::Duplicate(UObject* InOuter)
{
    UPointLightComponent* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();

    return NewComp;
}

void UPointLightComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
}

void UPointLightComponent::PostDuplicate()
{
}

FMatrix UPointLightComponent::GetViewMatrixForFace(int faceIndex) const
{
    FVector Up, Forward;

    switch (faceIndex)
    {
    case 0: // +X 방향
        Forward = FVector(1.0f, 0.0f, 0.0f);
        Up = FVector(0.0f, 1.0f, 0.0f);
        break;
    case 1: // -X 방향
        Forward = FVector(-1.0f, 0.0f, 0.0f);
        Up = FVector(0.0f, 1.0f, 0.0f);
        break;
    case 2: // +Y 방향
        Forward = FVector(0.0f, 1.0f, 0.0f);
        Up = FVector(0.0f, 0.0f, -1.0f);
        break;
    case 3: // -Y 방향
        Forward = FVector(0.0f, -1.0f, 0.0f);
        Up = FVector(0.0f, 0.0f, 1.0f);
        break;
    case 4: // +Z 방향
        Forward = FVector(0.0f, 0.0f, 1.0f);
        Up = FVector(0.0f, 1.0f, 0.0f);
        break;
    case 5: // -Z 방향
        Forward = FVector(0.0f, 0.0f, -1.0f);
        Up = FVector(0.0f, 1.0f, 0.0f);
        break;
    }
    return JungleMath::CreateViewMatrix(
        GetWorldLocation(),
        Forward + GetWorldLocation(),
        Up
    );
}

FMatrix UPointLightComponent::GetProjectionMatrix() const
{
    // 포인트 라이트는 항상 90도(큐브맵의 각 면)
    const float cubeFaceAngle = JungleMath::DegToRad(90.0f);

    // 1:1 종횡비 (정사각형)
    const float aspectRatio = 1.0f;

    // 근거리/원거리 평면
    const float nearPlane = 0.1f;
    const float farPlane = Radius;

    return JungleMath::CreateProjectionMatrix(
        cubeFaceAngle,
        aspectRatio,
        nearPlane,
        farPlane
    );
}

std::unique_ptr<FActorComponentInfo> UPointLightComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FPointLightComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void UPointLightComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FPointLightComponentInfo* Info = static_cast<FPointLightComponentInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);

    Info->Radius = Radius;
    Info->AttenuationFalloff = AttenuationFalloff;

}

void UPointLightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FPointLightComponentInfo& PointLightInfo = static_cast<const FPointLightComponentInfo&>(Info);
    Radius = PointLightInfo.Radius;
    AttenuationFalloff = PointLightInfo.AttenuationFalloff;
}
