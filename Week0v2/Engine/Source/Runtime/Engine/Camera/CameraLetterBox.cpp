#include "CameraLetterBox.h"

#include "EditorEngine.h"
#include "LaunchEngineLoop.h"

UCameraLetterBox::UCameraLetterBox()
    : TargetAspectRatio(-1.0f) // 기본적으로 비활성 상태 표시
{
    // 레터박스는 다른 효과(예: Fade)보다 먼저 적용되거나 후에 적용될 수 있음. 우선순위 조정 가능.
    // Priority = 50;
}

void UCameraLetterBox::ActivateLetterbox(float TargetRatio, float InDuration)
{
    if (TargetRatio > 0.0f) // 유효한 비율인지 확인
    {
        TargetAspectRatio = TargetRatio;
        // AlphaInTime은 부모 클래스 멤버
        AlphaInTime = FMath::Max(0.001f, InDuration); // 0 또는 음수 방지
        EnableModifier(); // 모디파이어 활성화 (Alpha가 0에서 1로 블렌딩 시작)
    }
}



void UCameraLetterBox::DeactivateLetterbox(float OutDuration)
{
    // AlphaOutTime은 부모 클래스 멤버
    AlphaOutTime = FMath::Max(0.001f, OutDuration); // 0 또는 음수 방지
    DisableModifier(false); // 모디파이어 비활성화 시작 (Alpha가 1에서 0으로 블렌딩 시작)
}

void UCameraLetterBox::ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings)
{
    // 1. 현재 화면(뷰포트) 크기 가져오기
    //    엔진 구현에 따라 PlayerCameraManager 또는 다른 곳에서 가져와야 함
    float ViewportWidth = 1920.0f; // 기본값
    float ViewportHeight = 1080.0f; // 기본값
    if (CameraOwner) // CameraOwner (APlayerCameraManager)가 유효한지 확인
    {        
        ViewportWidth = GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenWidth;
        ViewportHeight = GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenHeight;
    }

    // 0으로 나누기 방지
    if (ViewportHeight <= 0.0f || ViewportWidth <= 0.0f || TargetAspectRatio <= 0.0f)
    {
        // 유효하지 않은 값이면 효과 적용 안 함
        PostProcessSettings.LetterboxSize = 0.0f; 
        PostProcessSettings.PillarboxSize = 0.0f;
        PostProcessBlendWeight = 0.0f; 
        return;
    }

    // 2. 네이티브 화면 비율 계산
    const float NativeAspectRatio = ViewportWidth / ViewportHeight;

    // 3. 현재 Alpha 값을 사용하여 네이티브 비율과 목표 비율 사이를 보간
    //    Alpha = 0 이면 NativeAspectRatio, Alpha = 1 이면 TargetAspectRatio
    const float CurrentAspectRatio = FMath::Lerp(NativeAspectRatio, TargetAspectRatio, Alpha);
    
    // 4. 보간된 현재 비율(CurrentAspectRatio)을 기준으로 Letterbox 또는 Pillarbox 크기 계산
    float LetterboxSize = 0.0f;
    float PillarboxSize = 0.0f; 

    // if (FMath::IsNearlyEqual(NativeAspectRatio, CurrentAspectRatio))
    // {
    //     // 비율이 거의 같으면 바 없음
    //     LetterboxSize = 0.0f;
    //     PillarboxSize = 0.0f;
    // }
    if (NativeAspectRatio < CurrentAspectRatio)
    {
        // 네이티브가 더 좁음 (세로가 김) -> 레터박스 필요 (상하 바)
        const float TargetHeight = ViewportWidth / CurrentAspectRatio;
        const float TotalBarHeight = ViewportHeight - TargetHeight;
        LetterboxSize = (TotalBarHeight * 0.5f) / ViewportHeight; // 정규화 (0~0.5)
    }
    else // NativeAspectRatio > CurrentAspectRatio
    {
        // 네이티브가 더 넓음 (가로가 김) -> 필러박스 필요 (좌우 바)
        const float TargetWidth = ViewportHeight * CurrentAspectRatio;
        const float TotalBarWidth = ViewportWidth - TargetWidth;
        PillarboxSize = (TotalBarWidth * 0.5f) / ViewportWidth; // 정규화 (0~0.5)
    }

    PostProcessSettings.LetterboxSize = FMath::Clamp(LetterboxSize, 0.0f, 0.5f); // 가상 멤버
    PostProcessSettings.PillarboxSize = FMath::Clamp(PillarboxSize, 0.0f, 0.5f); // 가상 멤버

    PostProcessBlendWeight = 1.0f;
}