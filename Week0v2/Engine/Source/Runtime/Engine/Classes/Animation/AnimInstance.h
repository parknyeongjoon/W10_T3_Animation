#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "AnimTypes.h"
#include "AnimNodeBase.h"
#include "Animation/AnimSequence.h"
#include "AnimTypes.h"

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
    UAnimInstance(const UAnimInstance& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
    
    // APawn* TryGetPawnOwner() const;
    AActor* GetOwningActor() const;
    USkeletalMeshComponent* GetOwningComponent() const;
    void SetSkeleton(USkeleton* InSkeleton) { Skeleton = InSkeleton; }

    bool IsPlayingSlotAnimation(const UAnimSequenceBase* Asset, FName SlotNodeName) const;
    void StopSlotAnimation(float InBlendOutTime = 0.25f, FName SlotNodeName = "Empty") const;

    /** Gets the index of the state machine matching MachineName */
    // int32 GetStateMachineIndex(FName MachineName) const;
    /** Gets the runtime instance of the specified state machine */
    // const FAnimNode_StateMachine* GetStateMachineInstance(int32 MachineIndex) const;
    
    void TriggerAnimNotifies(float DeltaSeconds) const;
    void UpdateCurveValues(float DeltaSeconds) const ;

    virtual void NativeUpdateAnimation(float DeltaSeconds) const;
    
    void UpdateAnimation(UAnimSequence* AnimSequence, float DeltaTime);
    void BlendAnimations(UAnimSequence* FromSequence, UAnimSequence* ToSequence,float DeltaTime);
protected:
    UAnimSequence* CurrentSequence = nullptr;
    UAnimSequence* PreviousSequence = nullptr;  


    float BlendTime = 0;
    float BlendDuration = 5.0f;
    bool bIsBlending = false;

private:
    USkeleton* Skeleton;
    FBlendedCurve BlendedCurve;
    float CurrentTime = 0;

};

