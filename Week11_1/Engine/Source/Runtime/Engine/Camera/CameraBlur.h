#pragma once
#include "UCameraModifier.h"

class CameraBlur : public UCameraModifier
{
    DECLARE_CLASS(CameraBlur, UCameraModifier)
public:
    CameraBlur();
    
    // 블러 강도 설정 (0.0 ~ 1.0)
    float TargetBlurStrength = 0.5f;
    // 블러 반경 (픽셀 단위)
    float BlurRadius = 2.0f;
    
    // 블러 효과를 즉시 설정
    void SetBlurStrength(float NewStrength);
    // 블러 효과를 서서히 증가시킴
    void FadeInBlur(float TargetStrength = 1.0f, float FadeTime = 0.5f);
    // 블러 효과를 서서히 감소시킴
    void FadeOutBlur(float FadeTime = 0.5f);
    
    void ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings) override;
    
    float GetTargetAlpha() const override { return TargetBlurStrength; }
};
