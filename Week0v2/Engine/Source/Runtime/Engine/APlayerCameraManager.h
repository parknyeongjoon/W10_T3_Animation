#pragma once
#include "Camera/CameraDefine.h"
#include "Container/Array.h"
#include "GameFramework/Actor.h"

class UCameraModifier;
enum EViewTargetBlendOrder : int
{
    VTBlendOrder_Base,
    VTBlendOrder_Override
};

class APlayerCameraManager : public AActor
{
    DECLARE_CLASS(APlayerCameraManager, AActor)
public:
    APlayerCameraManager();
    APlayerCameraManager(const APlayerCameraManager& Other) {};
    virtual ~APlayerCameraManager() override {};

    UObject* Duplicate() const override;
    
    AActor* GetViewTarget() const { return ViewTarget.Target; }
    FSimpleViewInfo& GetViewInfo() { return ViewTarget.ViewInfo; }
    void AssignViewTarget(const FTViewTarget& InViewTarget)
    {
        ViewTarget = InViewTarget;
    }
    
    void AddCameraModifier(UCameraModifier* Modifier) { CameraModifiers.Add(Modifier); }
    void RemoveCameraModifier(UCameraModifier* Modifier) { CameraModifiers.Remove(Modifier); }
    void CleanCameraModifiers() { CameraModifiers.Empty(); }
    void ApplyCameraModifiers(float DeltaTime, FSimpleViewInfo& ViewInfo);

    /** Adds a postprocess effect at the given weight. */
    void AddCachedPPBlend(FPostProcessSettings& PPSettings, float BlendWeight, EViewTargetBlendOrder BlendOrder = VTBlendOrder_Base);
    void ClearCachedPPBlends();
    /** Returns active post process info. */
    void GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights) const;
    void GetCachedPostProcessBlends(TArray<FPostProcessSettings> const*& OutPPSettings, TArray<float> const*& OutBlendWeights, TArray<EViewTargetBlendOrder> const*& OutBlendOrders) const;
private:
    FTViewTarget ViewTarget;
    TArray<UCameraModifier*> CameraModifiers;
    
    TArray<FPostProcessSettings> PostProcessBlendCache;
    TArray<float> PostProcessBlendCacheWeights;
    TArray<EViewTargetBlendOrder> PostProcessBlendCacheOrders;
};
