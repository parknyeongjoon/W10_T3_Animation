#include "FPlayerCameraManager.h"
#include "Camera/UCameraModifier.h"

void FPlayerCameraManager::ApplyCameraModifiers(float DeltaTime, FViewInfo& ViewInfo)
{
    for (auto CameraModifier : CameraModifiers)
    {
        if (CameraModifier && !CameraModifier->IsDisabled())
        {
            CameraModifier->ModifyCamera(DeltaTime, ViewInfo);
        }
    }
}

void FPlayerCameraManager::AddCachedPPBlend(struct FPostProcessSettings& PPSettings, float BlendWeight, EViewTargetBlendOrder BlendOrder)
{
    PostProcessBlendCache.Add(PPSettings);
    PostProcessBlendCacheWeights.Add(BlendWeight);
    PostProcessBlendCacheOrders.Add(BlendOrder);
}

void FPlayerCameraManager::ClearCachedPPBlends()
{
    PostProcessBlendCache.Empty();
    PostProcessBlendCacheWeights.Empty();
    PostProcessBlendCacheOrders.Empty();
}

void FPlayerCameraManager::GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights) const
{
    OutPPSettings = &PostProcessBlendCache;
    OutBlendWeights = &PostProcessBlendCacheWeights;
}

void FPlayerCameraManager::GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights, TArray<EViewTargetBlendOrder> const*& OutBlendOrders) const
{
    OutPPSettings = &PostProcessBlendCache;
    OutBlendWeights = &PostProcessBlendCacheWeights;
    OutBlendOrders = &PostProcessBlendCacheOrders;
}
