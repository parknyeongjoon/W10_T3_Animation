#include "AnimInstance.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "UObject/Casts.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimNotify/AnimNotify.h"

UObject* UAnimInstance::Duplicate(UObject* InOuter)
{
    UAnimInstance* NewComp = FObjectFactory::ConstructObjectFrom<UAnimInstance>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

AActor* UAnimInstance::GetOwningActor() const
{
    return GetOwningComponent()->GetOwner();
}

USkeletalMeshComponent* UAnimInstance::GetOwningComponent() const
{
    return CastChecked<USkeletalMeshComponent>(GetOuter());
}
void UAnimInstance::TriggerAnimNotifies(float DeltaSeconds) const
{
    if (CurrentSequence)
    {
        for (const FAnimNotifyEvent& Notify : CurrentSequence->Notifies)
        {
            // 시간 조건에 맞으면 Notify 실행
            if (Notify.TriggerTime <= CurrentTime && CurrentTime < Notify.TriggerTime + Notify.Duration)
            {
                if (Notify.Notify)
                {
                    Notify.Notify->Notify(GetOwningComponent(), CurrentSequence);
                }
            }
        }
    }
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    TriggerAnimNotifies(DeltaSeconds);
    UpdateCurveValues(DeltaSeconds);
}

void UAnimInstance::UpdateCurveValues(float DeltaSeconds) const
{
    if (CurrentSequence)
    {
        // 현재 애니메이션 시퀀스에서 커브 정보를 가져옴
        //for (const FCurve& Curve : CurrentSequence->Curves)
        //{
        //    // 시간에 따른 커브 값 평가
        //    float Value = Curve.Evaluate(CurrentTime);

        //    // BlendedCurve에 저장
        //    BlendedCurve.SetCurveValue(Curve.CurveName, Value);
        //}
    }
}
