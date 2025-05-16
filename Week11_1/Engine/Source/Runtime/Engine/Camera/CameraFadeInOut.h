#pragma once
#include "UCameraModifier.h"
#include "D3D11RHI/CBStructDefine.h"


/**
 * @brief 화면 전체를 특정 색상으로 점진적으로 덮거나(Fade Out)
 *        덮인 상태에서 원래 화면으로 점진적으로 되돌리는(Fade In) 카메라 모디파이어입니다.
 *        주로 PostProcess 단계에 개입하여 오버레이 색상을 조절합니다.
 */
class UCameraFadeInOut : public UCameraModifier
{
    DECLARE_CLASS(UCameraFadeInOut, UCameraModifier)

public:
    /**
     * @brief 기본 생성자. 기본 페이드 색상을 검은색으로 설정합니다.
     */
    UCameraFadeInOut();

    /**
     * @brief 지정된 시간 동안 화면을 특정 색상으로 점진적으로 덮습니다 (Fade Out).
     * @param Duration 페이드 아웃 지속 시간 (초).
     * @param Color 페이드될 목표 색상 (기본값: 검은색).
     */
    void StartFadeOut(float Duration, FLinearColor Color = FLinearColor::Black);

    /**
     * @brief 지정된 시간 동안 덮여 있던 화면을 원래대로 점진적으로 되돌립니다 (Fade In).
     * @param Duration 페이드 인 지속 시간 (초).
     */
    void StartFadeIn(float Duration);

    /**
     * @brief 현재 페이드가 완료되었는지 (즉, 알파가 목표값에 도달했는지) 확인합니다.
     * @return 페이드 아웃 완료 시 true (Alpha == 1.0), 페이드 인 완료 시 true (Alpha == 0.0 && bDisabled)
     */
    bool IsFadeComplete() const;

protected:
    /**
     * @brief 포스트 프로세싱 설정을 수정하여 페이드 효과를 적용합니다.
     *        베이스 클래스의 가상 함수를 오버라이드합니다.
     * @param DeltaTime 프레임 간 시간.
     * @param PostProcessBlendWeight 이 모디파이어의 포스트 프로세스 효과 가중치 (Alpha 값 사용).
     * @param PostProcessSettings 수정할 포스트 프로세스 설정 구조체.
     */
    virtual void ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings) override;

private:
    /** @brief 페이드 효과에 사용될 색상입니다. */
    FLinearColor FadeColor;

    // Alpha, AlphaInTime, AlphaOutTime 등은 부모 클래스(UCameraModifier)의 것을 사용합니다.
};