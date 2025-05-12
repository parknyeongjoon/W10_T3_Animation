#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "AnimTypes.h"
#include "AnimNodeBase.h"
#include "Animation/AnimSequence.h"

class USkeleton;
class UAnimSequenceBase;
class USkeletalMeshComponent;
class AActor;
class FName;

class USkeletalMeshComponent;
class UAnimSequence;
class UAnimInstance : public UObject
{
    DECLARE_CLASS(UAnimInstance, UObject)
public:
    UAnimInstance() = default;
    UAnimInstance(const UAnimInstance&) = default;

    UObject* Duplicate(UObject* InOuter) override;

    // APawn* TryGetPawnOwner() const;
    AActor* GetOwningActor() const;
    USkeletalMeshComponent* GetOwningComponent() const;

    bool IsPlayingSlotAnimation(const UAnimSequenceBase* Asset, FName SlotNodeName) const;
    void StopSlotAnimation(float InBlendOutTime = 0.25f, FName SlotNodeName = "Empty") const;

    /** Gets the index of the state machine matching MachineName */
    // int32 GetStateMachineIndex(FName MachineName) const;
    /** Gets the runtime instance of the specified state machine */
    // const FAnimNode_StateMachine* GetStateMachineInstance(int32 MachineIndex) const;
    
    void TriggerAnimNotifies(float DeltaSeconds) const;
    void UpdateCurveValues(float DeltaSeconds) const ;
    // 테스트 코드
    UAnimSequence* AnimA = FObjectFactory::ConstructObject<UAnimSequence>(nullptr);
    UAnimSequence* AnimB = FObjectFactory::ConstructObject<UAnimSequence>(nullptr);
protected:
    virtual void NativeUpdateAnimation(float DeltaSeconds) const;

private:
    USkeleton* Skeleton;
    FBlendedCurve BlendedCurve;

    UAnimSequence* CurrentSequence = nullptr;

    float CurrentTime;
};

