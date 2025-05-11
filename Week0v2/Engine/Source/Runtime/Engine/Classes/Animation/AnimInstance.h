[pragma once
#include "Skeleton.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UAnimSequence;
class UAnimSequenceBase;
class USkeletalMeshComponent;
class AActor;
class FName;

class UAnimInstance : public UObject
{
    DECLARE_CLASS(UAnimInstance, UObject)
public:
    UAnimInstance() = default;

    // APawn* TryGetPawnOwner() const;
    AActor* GetOwningActor() const;
    USkeletalMeshComponent* GetOwningComponent() const;
    void SetOwningComponent(USkeletalMeshComponent* InComponent) { OwningComponent = InComponent; }
    bool IsPlayingSlotAnimation(const UAnimSequenceBase* Asset, FName SlotNodeName) const;
    void StopSlotAnimation(float InBlendOutTime = 0.25f, FName SlotNodeName = "Empty") const;

    /** Gets the index of the state machine matching MachineName */
    // int32 GetStateMachineIndex(FName MachineName) const;
    /** Gets the runtime instance of the specified state machine */
    // const FAnimNode_StateMachine* GetStateMachineInstance(int32 MachineIndex) const;
    
    void TriggerAnimNotifies(float DeltaSeconds) const;

protected:
    virtual void NativeUpdateAnimation(float DeltaSeconds) const;

private:
    USkeletalMeshComponent* OwningComponent;
};

