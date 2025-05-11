#pragma once
#include "Animation/AnimInstance.h"

class UAnimationStateMachine;
class UAnimSequence;

enum class ETestState
{
    Idle,
    Walking,
    Running,
};



class UTestAnimInstance :
    public UAnimInstance
{
public:
    UTestAnimInstance();
    ~UTestAnimInstance();

    virtual void TriggerAnimNotifies(float DeltaSeconds) override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
    UAnimationStateMachine* AnimStateMachine = nullptr;

    UAnimSequence* CurrentSequence = nullptr;
    UAnimSequence* IdleSequence = nullptr;
    UAnimSequence* WalkSequence = nullptr;
    UAnimSequence* RunSequence = nullptr;

    float Speed;
};

