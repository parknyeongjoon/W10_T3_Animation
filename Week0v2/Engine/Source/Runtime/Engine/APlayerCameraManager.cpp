#include "APlayerCameraManager.h"
#include "Camera/UCameraModifier.h"

APlayerCameraManager::APlayerCameraManager()
{
    auto SceneComp = AddComponent<USceneComponent>(EComponentOrigin::Constructor);
    RootComponent = SceneComp;
}

APlayerCameraManager::APlayerCameraManager(const APlayerCameraManager& Other)
    : AActor(Other),
    CameraModifiers(Other.CameraModifiers),
    ViewTarget(Other.ViewTarget),
    PostProcessBlendCache(Other.PostProcessBlendCache),
    PostProcessBlendCacheWeights(Other.PostProcessBlendCacheWeights),
    PostProcessBlendCacheOrders(Other.PostProcessBlendCacheOrders)
{
}


UObject* APlayerCameraManager::Duplicate() const
{
    APlayerCameraManager* ClonedActor = FObjectFactory::ConstructObjectFrom<APlayerCameraManager>(this);
    ClonedActor->DuplicateSubObjects(this);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void APlayerCameraManager::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

    // ToDo: 뷰정보 넣어주기
    FViewInfo ViewInfo;
    ApplyCameraModifiers(DeltaTime, ViewInfo);


}

void APlayerCameraManager::ApplyCameraModifiers(float DeltaTime, FViewInfo& ViewInfo)
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
