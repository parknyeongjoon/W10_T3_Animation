#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "CameraDefine.h"
#include "Math/Rotator.h"

class APlayerCameraManager;

class UCameraModifier : public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    UCameraModifier() = default;
    
    uint8 Priority = 0;

    //void Initialize(APlayerCameraManager* InCameraOwner, float InAlphaInTime, float InAlphaOutTime);

    void AddedToCamera(APlayerCameraManager* Camera);
    AActor* GetViewTarget() const;
    float GetTargetAlpha() const;
    void UpdateAlpha(float DeltaTime);

    virtual bool IsDisabled() const { return bDisabled; };
    virtual bool IsPendingDisable() const { return bPendingDisable; };
    virtual void DisableModifier(bool bImmediate = false);
    virtual void EnableModifier();
    
    virtual void ModifyCamera(float DeltaTime, FViewInfo& ViewInfo);
    virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, const FRotator& ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) {}
    virtual void ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings) {}
protected:
    APlayerCameraManager* CameraOwner = nullptr;
    
    float AlphaInTime = 0;
    bool bDisabled = false;
    bool bPendingDisable = false;

    float Alpha = 0;
    float AlphaOutTime = 0;
private:
};
