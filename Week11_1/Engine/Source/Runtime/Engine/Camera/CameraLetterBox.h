#pragma once
#include "Camera/UCameraModifier.h" // 베이스 클래스 UCameraModifier
/**
 * @brief 화면에 레터박스(상하 검은 바) 또는 필러박스(좌우 검은 바)를 추가하여
 *        목표 화면 비율(Aspect Ratio)을 강제하는 카메라 모디파이어입니다.
 *        활성화/비활성화 시 지정된 시간에 걸쳐 부드럽게 전환됩니다.
 */
class UCameraLetterBox : public UCameraModifier
{
    DECLARE_CLASS(UCameraLetterBox, UCameraModifier)

public:
    /**
     * @brief 기본 생성자. 기본 목표 비율을 -1.0f (비활성)로 설정합니다.
     */
    UCameraLetterBox();

    /**
     * @brief 지정된 목표 화면 비율로 레터박스/필러박스를 활성화합니다.
     *        효과는 지정된 시간 동안 부드럽게 나타납니다.
     * @param TargetRatio 목표 화면 비율 (예: 2.35f for 2.35:1).
     * @param InDuration 활성화 전환 시간 (초).
     */
    void ActivateLetterbox(float TargetRatio, float InDuration);
    void AddRenderObjectsToRenderPass(UWorld* InWorld);

    /**
     * @brief 활성화된 레터박스/필러박스를 비활성화합니다.
     *        효과는 지정된 시간 동안 부드럽게 사라져 원래 화면 비율로 돌아갑니다.
     * @param OutDuration 비활성화 전환 시간 (초).
     */
    void DeactivateLetterbox(float OutDuration);

    /**
     * @brief 현재 레터박스 효과의 목표 화면 비율을 반환합니다.
     * @return 목표 화면 비율. 비활성화 상태이거나 설정되지 않았다면 음수 값일 수 있습니다.
     */
    float GetTargetAspectRatio() const { return TargetAspectRatio; }

protected:
    /**
     * @brief 포스트 프로세싱 설정을 수정하여 레터박스/필러박스 효과를 적용합니다.
     *        현재 Alpha 값에 따라 목표 비율과 네이티브 비율 사이를 보간하고,
     *        계산된 LetterboxSize와 PillarboxSize를 FPostProcessSettings에 전달합니다.
     * @param DeltaTime 프레임 간 시간.
     * @param PostProcessBlendWeight 이 모디파이어의 포스트 프로세스 효과 가중치 (여기서는 1.0 고정).
     * @param PostProcessSettings 수정할 포스트 프로세스 설정 구조체.
     */
    virtual void ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings) override;

private:
    /** @brief 활성화 시 도달할 목표 화면 비율입니다. */
    float TargetAspectRatio;

    // Alpha, AlphaInTime, AlphaOutTime 등은 부모 클래스(UCameraModifier)의 것을 사용합니다.
    // Alpha 값이 0.0 (완전 비활성) 에서 1.0 (완전 활성) 사이를 보간하며,
    // 이 값을 ModifyPostProcess에서 사용하여 현재 적용될 화면 비율을 계산합니다.
};