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

    void AddedToCamera(APlayerCameraManager* Camera);
    AActor* GetViewTarget() const;
    virtual float GetTargetAlpha() const;
    void UpdateAlpha(float DeltaTime);

    virtual bool IsDisabled() const { return bDisabled; };
    virtual bool IsPendingDisable() const { return bPendingDisable; };
    virtual void DisableModifier(bool bImmediate = false);
    virtual void EnableModifier();
    
    virtual void ModifyCamera(float DeltaTime, FSimpleViewInfo& ViewInfo);
    virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, const FRotator& ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) {}
    virtual void ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings) {}
protected:
    APlayerCameraManager* CameraOwner = nullptr;

    float Alpha = 0;
    float AlphaInTime = 0;
    float AlphaOutTime = 0;
    
private:
    bool bDisabled = false;
    bool bPendingDisable = false;
};
