#include "AnimationStateMachine.h"
void UAnimationStateMachine::AddState(const FString& StateName, std::function<void(float)> OnUpdate)
{
    States.Emplace(StateName, OnUpdate);
}

void UAnimationStateMachine::AddTransition(const FString& FromState, const FString& ToState, std::function<bool()> Condition)
{
    TPair<FString, FString> Key(FromState, ToState);
    Transitions.Emplace(Key, Condition);
}

void UAnimationStateMachine::SetState(const FString& NewState)
{
    if (States.Contains(NewState))
    {
        CurrentState = NewState;
        std::cout << "State Changed to: " << *NewState << std::endl;
    }
}

void UAnimationStateMachine::Update(float DeltaTime)
{
    // 현재 상태 실행
    if (States.Contains(CurrentState))
    {
        States[CurrentState](DeltaTime);
    }

    // Transition 조건 평가
    for (auto& Transition : Transitions)
    {
        const TPair<FString, FString>& Key = Transition.Key;
        const std::function<bool()>& Condition = Transition.Value;

        if (Key.Key == CurrentState && Condition())
        {
            SetState(Key.Value);
            break;
        }
    }
}

FString UAnimationStateMachine::GetCurrentState() const
{
    return CurrentState;
}