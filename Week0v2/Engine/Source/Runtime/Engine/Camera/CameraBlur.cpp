#include "CameraBlur.h"

CameraBlur::CameraBlur()
{
    Priority = 1; // 우선순위 설정 (높을수록 나중에 적용)
    AlphaInTime = 0.3f;
    AlphaOutTime = 0.3f;
    
    // 블러 기본값 초기화
    TargetBlurStrength = 0.5f;
    BlurRadius = 2.0f;
}

void CameraBlur::SetBlurStrength(float NewStrength)
{
    // 블러 강도를 0.0~1.0 사이로 제한
    TargetBlurStrength = FMath::Clamp(NewStrength, 0.0f, 1.0f);
    
    // 블러 강도가 0이면 모디파이어를 비활성화
    if (FMath::IsNearlyEqual(TargetBlurStrength, 0, 0.01f))
    {
        DisableModifier(true); // 즉시 비활성화
    }
    else if (IsDisabled()) // 블러 강도가 0이 아니고 비활성화 상태라면
    {
        EnableModifier(); // 활성화
    }
}

void CameraBlur::FadeInBlur(float TargetStrength, float FadeTime)
{
    // 블러 강도 설정
    TargetBlurStrength = FMath::Clamp(TargetStrength, 0.0f, 1.0f);
    
    // 페이드 시간 설정
    if (FadeTime > 0.0f)
    {
        // AlphaInTime 조정
        AlphaInTime = FadeTime;
    }
    
    // 모디파이어 활성화 - Alpha는 AlphaInTime에 따라 0에서 TargetBlurStrength로 증가
    EnableModifier();
}

void CameraBlur::FadeOutBlur(float FadeTime)
{
    // 페이드 시간 설정
    if (FadeTime > 0.0f)
    {
        // AlphaOutTime 조정
        AlphaOutTime = FadeTime;
    }
    
    // 모디파이어 비활성화 - Alpha는 AlphaOutTime에 따라 현재 값에서 0으로 감소
    DisableModifier(FadeTime <= 0.0f); // 페이드 시간이 0이면 즉시 비활성화
}

void CameraBlur::ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings)
{
    if (Alpha > 0.0f)
    {
        PostProcessSettings.bOverride_GaussianBlur = 1;
        PostProcessSettings.GaussianBlurStrength = Alpha; // Alpha 값 자체가 블러 강도가 됨
        PostProcessSettings.GaussianBlurRadius = BlurRadius;
        
        // 블렌드 가중치 설정
        PostProcessBlendWeight = 1.0f;
    }
    else
    {
        // 모디파이어가 비활성화된 경우 블러 효과 적용 안 함
        PostProcessSettings.bOverride_GaussianBlur = 0;
        PostProcessBlendWeight = 0.0f;
    }
}
