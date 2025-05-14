#pragma once
#include "Animation/AnimInstance.h"
#include "Delegates/FFunctor.h"

class UAnimSequence;
template <typename T>
class UAnimationStateMachine;

enum class ETestState
{
    Idle,
    Walking,
    Dancing,
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
    //virtual void TriggerAnimNotifies(float DeltaSeconds) override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
    UAnimationStateMachine<ETestState>* AnimStateMachine = nullptr;

    UAnimSequence* IdleSequence = nullptr;
    UAnimSequence* WalkSequence = nullptr;
    UAnimSequence* DanceSequence = nullptr;

    StateCallback IdleCallback = StateCallback(this);
    StateCallback WalkCallback = StateCallback(this);
    StateCallback DanceCallback = StateCallback(this);
};

