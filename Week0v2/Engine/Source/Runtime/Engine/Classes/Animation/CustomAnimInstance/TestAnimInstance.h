#pragma once
#include "Animation/AnimationStateMachine.h"
#include "Animation/AnimInstance.h"
#include "Delegates/FFunctor.h"

class UAnimSequence;
template <typename T>
class UAnimationStateMachine;

enum class ETestState : uint8
{
    None,
    Pose,
    Jump,
    Dance,
    Defeated,
    Max
};

namespace std {
    template <>
    struct hash<ETestState>
    {
        std::size_t operator()(const ETestState& state) const noexcept
        {
            return static_cast<std::size_t>(state);
        }
    };
}

namespace std {
    template <>
    struct hash<TPair<ETestState, ETestState>>
    {
        std::size_t operator()(const TPair<ETestState, ETestState>& pair) const noexcept
        {
            std::size_t h1 = std::hash<ETestState>{}(pair.Key);
            std::size_t h2 = std::hash<ETestState>{}(pair.Value);
            return h1 ^ (h2 << 1); 
        }
    };
}

class UTestAnimInstance :
    public UAnimInstance
{
    DECLARE_CLASS(UTestAnimInstance, UAnimInstance)
    DECLARE_CONTEXT_FUNC(StateCallback, void, float);
public:
    UTestAnimInstance();
    UTestAnimInstance(const UTestAnimInstance& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    void SetState(ETestState NewState) const { AnimStateMachine->SetState(NewState); }
private:
    UAnimationStateMachine<ETestState>* AnimStateMachine = nullptr;

    UAnimSequence* StandingSequence = nullptr;
    UAnimSequence* JumpSequence = nullptr;
    UAnimSequence* DanceSequence = nullptr;
    UAnimSequence* DeafeatedSequence = nullptr;

    StateCallback StandingCallback = StateCallback(this);
    StateCallback JumpCallback = StateCallback(this);
    StateCallback DanceCallback = StateCallback(this);
    StateCallback DeafeatedCallback = StateCallback(this);
};

