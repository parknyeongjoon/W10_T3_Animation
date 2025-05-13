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
    UAnimInstance(const UAnimInstance& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    // APawn* TryGetPawnOwner() const;
    AActor* GetOwningActor() const;
    USkeletalMeshComponent* GetOwningComponent() const;

    bool IsPlayingSlotAnimation(const UAnimSequenceBase* Asset, FName SlotNodeName) const;
    void StopSlotAnimation(float InBlendOutTime = 0.25f, FName SlotNodeName = "Empty") const;

    /** Gets the index of the state machine matching MachineName */
    // int32 GetStateMachineIndex(FName MachineName) const;
    /** Gets the runtime instance of the specified state machine */
    // const FAnimNode_StateMachine* GetStateMachineInstance(int32 MachineIndex) const;

    //void AddAnimNotify(float Second, TDelegate<void()> OnNotify, float Duration = 0.1f) const;
    //void AddAnimNotify(float Second, std::function<void()> OnNotify, float Duration = 0.1f) const;
    //void DeleteAnimNotify(float Second) const;
    void TriggerAnimNotifies(float DeltaSeconds) const;
    void UpdateCurveValues(float DeltaSeconds) const ;

    virtual void NativeInitializeAnimation();
    virtual void NativeUpdateAnimation(float DeltaSeconds);
    void UpdateAnimation(UAnimSequence* AnimSequence, float DeltaTime);
    void BlendAnimations(UAnimSequence* FromSequence, UAnimSequence* ToSequence,float DeltaTime);
protected:
    UAnimSequence* CurrentSequence = nullptr;
    UAnimSequence* PreviousSequence = nullptr;  


    float BlendTime = 0;
    float BlendDuration = 0.5f;
    bool bIsBlending = false;

private:
    USkeleton* Skeleton;
    FBlendedCurve BlendedCurve;
    float CurrentTime = 0;

};

