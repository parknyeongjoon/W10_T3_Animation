#include "PlayerCameraManager.h"
#include "EditorEngine.h"
#include "Camera/UCameraModifier.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

void APlayerCameraManager::Tick(float DeltaTime)
{
    ClearCachedPPBlends();
    float DeltaTimeSecond = DeltaTime * 0.001f;
    UpdateViewTarget();
    ApplyCameraModifiers(DeltaTimeSecond, ViewTarget.ViewInfo);
    ApplyCameraShakes(DeltaTimeSecond, ViewTarget.ViewInfo);
    ApplyFinalViewToCamera();
}

APlayerCameraManager::APlayerCameraManager()
{
    auto SceneComp = AddComponent<USceneComponent>(EComponentOrigin::Constructor);
    RootComponent = SceneComp;
}

APlayerCameraManager::APlayerCameraManager(const APlayerCameraManager& Other)
    : AActor(Other)
    // CameraModifiers(Other.CameraModifiers),
    // ViewTarget(Other.ViewTarget),
    // PostProcessBlendCache(Other.PostProcessBlendCache),
    // PostProcessBlendCacheWeights(Other.PostProcessBlendCacheWeights),
    // PostProcessBlendCacheOrders(Other.PostProcessBlendCacheOrders)
{
    //GetWorld()->SetPlayerCameraManager(this);
}

void APlayerCameraManager::AddCameraModifier(UCameraModifier* Modifier)
{
    {
        Modifier->AddedToCamera(this);
        CameraModifiers.Add(Modifier);
    }
}

UObject* APlayerCameraManager::Duplicate(UObject* InOuter)
{
    APlayerCameraManager* ClonedActor = FObjectFactory::ConstructObjectFrom<APlayerCameraManager>(this, InOuter);
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void APlayerCameraManager::ApplyCameraModifiers(float DeltaTime, FSimpleViewInfo& ViewInfo)
{
    for (auto CameraModifier : CameraModifiers)
    {
        if (CameraModifier && !CameraModifier->IsDisabled())
        {
            CameraModifier->ModifyCamera(DeltaTime, ViewInfo);
        }
    }
}

void APlayerCameraManager::AddCachedPPBlend(struct FPostProcessSettings& PPSettings, float BlendWeight, EViewTargetBlendOrder BlendOrder)
{
    PostProcessBlendCache.Add(PPSettings);
    PostProcessBlendCacheWeights.Add(BlendWeight);
    PostProcessBlendCacheOrders.Add(BlendOrder);
}

void APlayerCameraManager::ClearCachedPPBlends()
{
    PostProcessBlendCache.Empty();
    PostProcessBlendCacheWeights.Empty();
    PostProcessBlendCacheOrders.Empty();
}

void APlayerCameraManager::GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights) const
{
    OutPPSettings = &PostProcessBlendCache;
    OutBlendWeights = &PostProcessBlendCacheWeights;
}

void APlayerCameraManager::GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights, TArray<EViewTargetBlendOrder> const*& OutBlendOrders) const
{
    OutPPSettings = &PostProcessBlendCache;
    OutBlendWeights = &PostProcessBlendCacheWeights;
    OutBlendOrders = &PostProcessBlendCacheOrders;
}

void APlayerCameraManager::StartCameraShake(UCameraShakeBase* Shake)
{
    if (!Shake) return;
    ActiveShakes.Add({ Shake });
}

void APlayerCameraManager::UpdateViewTarget()
{
    if (!ViewTarget.Target) return;

    if (UCameraComponent* Cam = ViewTarget.Target->GetComponentByClass<UCameraComponent>())
    {
        
        ViewTarget.ViewInfo.Location = Cam->GetRelativeLocation();
        ViewTarget.ViewInfo.Rotation = Cam->GetRelativeRotation();
        ViewTarget.ViewInfo.FOV = Cam->GetFOV();
    }
}

void APlayerCameraManager::ApplyCameraShakes(float DeltaTime, FSimpleViewInfo& ViewInfo)
{
    for (int32 i = ActiveShakes.Num() - 1; i >= 0; --i)
    {
        FActiveCameraShakeInfo& Info = ActiveShakes[i];
        if (Info.IsFinished())
        {
            Info.Instance->MarkRemoveObject();
            ActiveShakes.RemoveAt(i);
            continue;
        }

        FVector Loc; FRotator Rot; float FOV;
        Info.Instance->Tick(DeltaTime);
        Info.Instance->UpdateShake(DeltaTime, Loc, Rot, FOV);

        float W = Info.Instance->GetBlendWeight();
        ViewInfo.Location += Loc;
        ViewInfo.Rotation += Rot;
        ViewInfo.FOV += FOV;
    }
}

void APlayerCameraManager::ApplyFinalViewToCamera()
{
    if (!ViewTarget.Target) return;

    UCameraComponent* Cam = ViewTarget.Target->GetComponentByClass<UCameraComponent>();
    if (!Cam) return;

    const FSimpleViewInfo& View = ViewTarget.ViewInfo;

    Cam->SetRelativeLocation(View.Location);
    Cam->SetRelativeRotation(View.Rotation);
    Cam->SetFOV(View.FOV);
}
