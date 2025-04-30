#pragma once
#include "Camera/CameraDefine.h"
#include "Container/Array.h"

class UCameraModifier;
enum EViewTargetBlendOrder : int
{
    VTBlendOrder_Base,
    VTBlendOrder_Override
};

class FPlayerCameraManager
{
public:
    AActor* GetViewTarget() const { return ViewTarget.Target; }
    FViewInfo& GetViewInfo() { return ViewTarget.ViewInfo; }
    void AssignViewTarget(const FTViewTarget& InViewTarget) { ViewTarget = InViewTarget; }
    
    void AddCameraModifier(UCameraModifier* Modifier) { CameraModifiers.Add(Modifier); }
    void RemoveCameraModifier(UCameraModifier* Modifier) { CameraModifiers.Remove(Modifier); }
    void CleanCameraModifiers() { CameraModifiers.Empty(); }
    void ApplyCameraModifiers(float DeltaTime, FViewInfo& ViewInfo);

    /** Adds a postprocess effect at the given weight. */
    void AddCachedPPBlend(FPostProcessSettings& PPSettings, float BlendWeight, EViewTargetBlendOrder BlendOrder = VTBlendOrder_Base);
    void ClearCachedPPBlends();
    /** Returns active post process info. */
    void GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights) const;
    void GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights, TArray<EViewTargetBlendOrder> const*& OutBlendOrders) const;
private:
    FTViewTarget ViewTarget = FTViewTarget();
    TArray<UCameraModifier*> CameraModifiers;
    
    TArray<FPostProcessSettings> PostProcessBlendCache;
    TArray<float> PostProcessBlendCacheWeights;
    TArray<EViewTargetBlendOrder> PostProcessBlendCacheOrders;
};
