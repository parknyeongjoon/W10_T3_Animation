#include "UCameraModifier.h"
#include "PlayerCameraManager.h"

// void UCameraModifier::Initialize(APlayerCameraManager* InCameraOwner, float InAlphaInTime, float InAlphaOutTime)
// {
//     CameraOwner = InCameraOwner;
// }

void UCameraModifier::AddedToCamera(APlayerCameraManager* Camera)
{
    CameraOwner = Camera;
}

AActor* UCameraModifier::GetViewTarget() const
{
    return CameraOwner ? CameraOwner->GetViewTarget() : nullptr;
}

float UCameraModifier::GetTargetAlpha() const
{
    return bPendingDisable ? 0.0f : 1.f;
}

void UCameraModifier::UpdateAlpha(float DeltaTime)
{
    float const TargetAlpha = GetTargetAlpha();
    float const BlendTime = (TargetAlpha == 0.f) ? AlphaOutTime : AlphaInTime;

    // interpolate!
    if (BlendTime <= 0.f)
    {
        // no blendtime means no blending, just go directly to target alpha
        Alpha = TargetAlpha;
    }
    else if (Alpha > TargetAlpha)
    {
        // interpolate downward to target, while protecting against overshooting
        Alpha = FMath::Max<float>(Alpha - DeltaTime / BlendTime, TargetAlpha);
    }
    else
    {
        // interpolate upward to target, while protecting against overshooting
        Alpha = FMath::Min<float>(Alpha + DeltaTime / BlendTime, TargetAlpha);
    }
}

void UCameraModifier::DisableModifier(bool bImmediate)
{
    if (bImmediate)
    {
        bDisabled = true;
        bPendingDisable = false;
    }
    else if (!bDisabled)
    {
        bPendingDisable = true;
    }
}

void UCameraModifier::EnableModifier()
{
    bDisabled = false;
    bPendingDisable = false;
}

void UCameraModifier::ModifyCamera(float DeltaTime, FSimpleViewInfo& ViewInfo)
{
    UpdateAlpha(DeltaTime);
    ModifyCamera(DeltaTime, ViewInfo.Location, ViewInfo.Rotation, ViewInfo.FOV, ViewInfo.Location, ViewInfo.Rotation, ViewInfo.FOV);

    if (CameraOwner)
    {
        // note: pushing these through the cached PP blend system in the camera to get
        // proper layered blending, rather than letting subsequent mods stomp over each other in the 
        // InOutPOV struct.
        {
            float PPBlendWeight = 0.f;
            FPostProcessSettings PPSettings;
			
            //  Let native code modify the post process s ettings.
            ModifyPostProcess(DeltaTime, PPBlendWeight, PPSettings);
			
            if (PPBlendWeight > 0.f)
            {
                CameraOwner->AddCachedPPBlend(PPSettings, PPBlendWeight);
            }
        }
    }
    
    if (bPendingDisable && (Alpha <= 0.f))
    {
        DisableModifier(true);
    }
}