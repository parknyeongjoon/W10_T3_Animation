#pragma once
#include "Camera/CameraDefine.h"
#include "Container/Array.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraShake/CameraShakeBase.h"

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
    virtual void Tick(float DeltaTime) override;
public:
    APlayerCameraManager();
    APlayerCameraManager(const APlayerCameraManager& Other);
    virtual ~APlayerCameraManager() override {};

    UObject* Duplicate() override;
    
    AActor* GetViewTarget() const { return ViewTarget.Target; }
    FSimpleViewInfo& GetViewInfo() { return ViewTarget.ViewInfo; }
    void AssignViewTarget(const FTViewTarget& InViewTarget)
    {
        ViewTarget = InViewTarget;
    }

    
    void AddCameraModifier(UCameraModifier* Modifier);

    void RemoveCameraModifier(UCameraModifier* Modifier)
    {
        CameraModifiers.Remove(Modifier);
    }
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
public:
    void StartCameraShake(UCameraShakeBase* Shake);
private:
    TArray<FActiveCameraShakeInfo> ActiveShakes;

    void UpdateViewTarget();           // ViewTarget.Target → ViewInfo 갱신
    void ApplyCameraShakes(float DeltaTime, FSimpleViewInfo& ViewInfo);         // ActiveShake 계산
    void ApplyFinalViewToCamera(); // ViewInfo + 흔들림 → 실제 카메라 적용

};
