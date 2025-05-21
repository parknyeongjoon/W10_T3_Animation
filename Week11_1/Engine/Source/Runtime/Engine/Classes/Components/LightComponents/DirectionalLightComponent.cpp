#include "DirectionalLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"
#include "EditorEngine.h"
#include "Engine/World.h"
#include "Math/JungleMath.h"
#include "Define.h"
#include "LaunchEngineLoop.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"

extern UEngine* GEngine;

UDirectionalLightComponent::UDirectionalLightComponent()
{
    ShadowResource = new FShadowResource[CASCADE_COUNT];
    for (int i =0;i<CASCADE_COUNT;i++)
    {
        UINT temp = pow(2,4-i);
        FShadowResource* resource = FShadowResourceFactory::CreateShadowResource(GEngineLoop.GraphicDevice.Device, ELightType::DirectionalLight, 256 * temp, false);
        ShadowResource[i] = *resource;
        ShadowResources.Add(resource);
    }
}


UDirectionalLightComponent::~UDirectionalLightComponent()
{
    // release all resources
    ShadowResource = nullptr;
    for (int i = 0; i < ShadowResources.Num(); i++)
    {
        if (ShadowResources[i])
        {
            delete ShadowResources[i];
            ShadowResources[i] = nullptr;
        }
    }
    ShadowResources.Empty();
}

//void UDirectionalLightComponent::SetDirection(FVector _newDir)
//{
//    //잘 안됨
//    FVector Axis = Direction.Cross(_newDir).Normalize();
//    float Angle = acosf(Direction.Normalize().Dot(_newDir.Normalize()));
//    GetOwner()->GetRootComponent()->SetRelativeQuat(FQuat::FromAxisAngle(Axis, Angle));
//    Direction = _newDir;
//}
const float SCENE_RADIUS = 100.0f;
const float ZPaddingFactor = 1.5f; 
FMatrix UDirectionalLightComponent::GetViewMatrix() const
{
    // 광원 위치 결정 (씬의 중심에서 반대 방향으로)
    FVector sceneCenter = FVector(0,0,0); // TODO: Scene Center 넣기
    FVector lightPos = sceneCenter - GetWorldForwardVector() * SCENE_RADIUS;
    // 광원 뷰 행렬 계산
    FVector upVector = FVector(0.0f, 0.0f, 1.0f);
    if (abs(GetWorldForwardVector().Dot(upVector) > 0.9f))
    {
        upVector = FVector(0.0f, 0.0f, 1.0f);
    }
    
    return JungleMath::CreateViewMatrix(
        lightPos,
        sceneCenter,
        upVector);
}

FMatrix UDirectionalLightComponent::GetCascadeViewMatrix(UINT CascadeIndex) const
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);

    if (EditorEngine == nullptr)
    {
        return FMatrix::Identity;
    }
    
    FVector* CascadeCorner = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetCascadeCorner(CascadeIndex);
    FVector center = FVector::ZeroVector;
    for (int i=0;i<8;i++)
    {
        center += CascadeCorner[i];
    }
    center /= 8.0f;

    FVector lightDir = GetWorldForwardVector().Normalize();
    FVector up = FVector(0.0f, 0.0f, 1.0f);
    if (abs(lightDir.Dot(up)) > 0.99f) {
        up = FVector(0.0f, 0.0f, 0.99f);
    }

    return JungleMath::CreateViewMatrix(
        center - lightDir * SCENE_RADIUS * ZPaddingFactor,
        center,
        up);
}

FMatrix UDirectionalLightComponent::GetProjectionMatrix() const
{
    // 직교 투영 행렬 계산 (방향광은 직교 투영 사용)
    return JungleMath::CreateOrthoProjectionMatrix(SCENE_RADIUS * 2,SCENE_RADIUS * 2, 0.1f, 4 * SCENE_RADIUS);
}

FMatrix UDirectionalLightComponent::GetCascadeProjectionMatrix(UINT CascadeIndex) const
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);

    if (EditorEngine == nullptr)
    {
        return FMatrix::Identity;
    }
    
    FVector* CascadeCorner = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetCascadeCorner(CascadeIndex);
    FMatrix viewMatrix = GetCascadeViewMatrix(CascadeIndex);

    FVector minExtents = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
    FVector maxExtents = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    
    for (int i = 0; i < 8; i++) {
        FVector lightSpaceCorner = viewMatrix.TransformPosition(CascadeCorner[i]);
        minExtents = lightSpaceCorner.Min(minExtents);
        maxExtents = lightSpaceCorner.Max(maxExtents);
    }

    // 경계 박스의 크기 계산
    float paddingFactor = 1.5f;
    float width = maxExtents.X - minExtents.X * paddingFactor;
    float height = maxExtents.Y - minExtents.Y * paddingFactor;
    float nearPlane, farPlane;

    // 중심점 계산
    float centerX = (maxExtents.X + minExtents.X) * 0.5f;
    float centerY = (maxExtents.Y + minExtents.Y) * 0.5f;
    
    // 새로운 경계 계산
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    float newMinX = centerX - halfWidth;
    float newMaxX = centerX + halfWidth;
    float newMinY = centerY - halfHeight;
    float newMaxY = centerY + halfHeight;
    
    // 정밀도를 위해 z 근평면과 원평면을 조정
    if (minExtents.Z < 0) {
        nearPlane = minExtents.Z * ZPaddingFactor;
    } else {
        nearPlane = minExtents.Z / ZPaddingFactor;
    }

    if (maxExtents.Z < 0) {
        farPlane = maxExtents.Z / ZPaddingFactor;
    } else {
        farPlane = maxExtents.Z * ZPaddingFactor;
    }

    return JungleMath::CreateOrthoProjectionMatrix(newMinX, newMaxX, 
        newMinY, newMaxY, nearPlane, farPlane);
}

UObject* UDirectionalLightComponent::Duplicate(UObject* InOuter)
{
    UDirectionalLightComponent* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();

    return NewComp;
}

void UDirectionalLightComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
    // 여기서 복사할 것? 방향뿐임
    UDirectionalLightComponent* SourceComp = Cast<UDirectionalLightComponent>(Source);
    SourceComp->Direction = Direction;
}

void UDirectionalLightComponent::PostDuplicate()
{
}

std::unique_ptr<FActorComponentInfo> UDirectionalLightComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FDirectionalLightComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void UDirectionalLightComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FDirectionalLightComponentInfo* Info = static_cast<FDirectionalLightComponentInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);
    
    Info->Direction = Direction;
}

void UDirectionalLightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FDirectionalLightComponentInfo& DirectionalLightInfo = static_cast<const FDirectionalLightComponentInfo&>(Info);
    Direction = DirectionalLightInfo.Direction;
}
