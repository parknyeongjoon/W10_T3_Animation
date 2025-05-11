#pragma once
#include "Animation/AnimInstance.h"

class UAnimSequence;
template <typename T>
class UAnimationStateMachine;

enum class ETestState
{
    Idle,
    Walking,
    Running,
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
public:
    UTestAnimInstance();
    ~UTestAnimInstance();

    virtual void TriggerAnimNotifies(float DeltaSeconds) override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
    UAnimationStateMachine<ETestState>* AnimStateMachine = nullptr;

    UAnimSequence* CurrentSequence = nullptr;
    UAnimSequence* IdleSequence = nullptr;
    UAnimSequence* WalkSequence = nullptr;
    UAnimSequence* RunSequence = nullptr;

    float Speed;
};

