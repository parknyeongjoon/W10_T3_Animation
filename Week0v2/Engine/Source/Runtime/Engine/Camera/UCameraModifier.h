#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "CameraDefine.h"
#include "Math/Rotator.h"

class FPlayerCameraManager;

class UCameraModifier : public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    UCameraModifier() {};
    
    uint8 Priority = 0;

    void AddedToCamera(FPlayerCameraManager* Camera);
    AActor* GetViewTarget() const;
    float GetTargetAlpha() const;
    void UpdateAlpha(float DeltaTime);

    virtual bool IsDisabled() const { return bDisabled; };
    virtual bool IsPendingDisable() const { return bPendingDisable; };
    virtual void DisableModifier(bool bImmediate = false);
    virtual void EnableModifier();
    
    virtual void ModifyCamera(float DeltaTime, FViewInfo& ViewInfo);
    virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) {}
    virtual void ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings) {}
protected:
    FPlayerCameraManager* CameraOwner = nullptr;
private:
    bool bDisabled = false;
    bool bPendingDisable = false;

    float Alpha = 0;
    float AlphaInTime = 0;
    float AlphaOutTime = 0;
};
