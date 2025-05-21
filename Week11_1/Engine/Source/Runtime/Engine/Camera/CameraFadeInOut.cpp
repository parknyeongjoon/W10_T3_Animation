#include "Camera/CameraFadeInOut.h" // 해당 클래스의 헤더 파일 포함
//#include "Camera/PlayerCameraManager.h" // 필요시 CameraOwner 관련 기능 사용
//#include "Math/UnrealMathUtility.h" // FMath 사용 (CoreMinimal에 포함될 수도 있음)
// #include "Rendering/PostProcessSettings.h" // 헤더에서 이미 포함됨


UCameraFadeInOut::UCameraFadeInOut()
{
    // 기본적으로 검은색으로 페이드하도록 설정
    FadeColor = FLinearColor::Black;
    // 이 모디파이어는 다른 효과들보다 나중에 적용되어야 할 수 있으므로 높은 우선순위 부여 가능
    Priority = 100; 
}

void UCameraFadeInOut::StartFadeOut(float Duration, FLinearColor Color)
{
    // 목표 색상 및 블렌드 시간 설정
    FadeColor = Color;
    AlphaInTime = FMath::Max(0.001f, Duration); // 0 또는 음수 방지
    Alpha = 0.0f; // 초기 알파값 설정 (1.0f로 시작)

    // 모디파이어 활성화 (Alpha가 0에서 1로 블렌딩 시작)
    EnableModifier();
}

void UCameraFadeInOut::StartFadeIn(float Duration)
{
    // 블렌드 아웃 시간 설정
    AlphaOutTime = FMath::Max(0.001f, Duration); // 0 또는 음수 방지
    Alpha = 1.0f;

    // 모디파이어 비활성화 시작 (Alpha가 1에서 0으로 블렌딩 시작)
    // bImmediate = false 이므로 부드럽게 사라짐
    DisableModifier(false);
}

bool UCameraFadeInOut::IsFadeComplete() const
{
    if (GetTargetAlpha() == 1.0f && Alpha == 1.0f) // Fade Out 완료 조건
    {
        return true;
    }
    if (IsDisabled() && Alpha == 0.0f) // Fade In 완료 조건 (비활성화 상태이고 알파가 0)
    {
        return true;
    }
    return false;
}

void UCameraFadeInOut::ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings)
{
    PostProcessSettings.FadeColor = FadeColor;
    PostProcessSettings.FadeAlpha = Alpha; // Alpha가 직접 강도를 조절
    PostProcessBlendWeight = 1.0f; // 이 경우 모디파이어 자체의 블렌드는 1로 고정하고, 내부 OverlayIntensity로 조절
}