#pragma once
#include "UObject/Object.h"
#include "Container/Array.h"
#include "Container/Map.h"

template <typename TState>
class UAnimationStateMachine : public UObject
{
    DECLARE_CLASS(UAnimationStateMachine, UObject)
public:
    UAnimationStateMachine() = default;
    UAnimationStateMachine(const UAnimationStateMachine& Other);
    virtual UObject* Duplicate(UObject* InOuter) override;

    void AddState(TState StateName, std::function<void(float)> OnUpdate)
    {
        States.Emplace(StateName, OnUpdate);
    }

    void AddTransition(TState FromState, TState ToState, std::function<bool()> Condition)
    {
        TPair<TState, TState> Key(FromState, ToState);
        Transitions.Emplace(Key, Condition);
    }

    void SetState(TState NewState)
    {
        if (States.Contains(NewState))
        {
            CurrentState = NewState;
        }
    }

    void Update(float DeltaTime)
    {
        if (States.Contains(CurrentState))
        {
            States[CurrentState](DeltaTime);
        }

        for (auto& Transition : Transitions)
        {
            const TPair<TState, TState>& Key = Transition.Key;
            const std::function<bool()>& Condition = Transition.Value;

            if (Key.Key == CurrentState && Condition())
            {
                SetState(Key.Value);
                break;
            }
        }
    }

    TState GetCurrentState() const
    {
        return CurrentState;
    }

private:
    TState CurrentState;
    TMap<TState, std::function<void(float)>> States;
    TMap<TPair<TState, TState>, std::function<bool()>> Transitions;
};

template<typename TState>
inline UAnimationStateMachine<TState>::UAnimationStateMachine(const UAnimationStateMachine& Other)
    :UObject(Other),
    CurrentState(Other.CurrentState)
    //States(Other.States),
    //Transitions(Other.Transitions)
{
}

template<typename TState>
inline UObject* UAnimationStateMachine<TState>::Duplicate(UObject* InOuter)
{
    UAnimationStateMachine<TState>* NewComp = FObjectFactory::ConstructObjectFrom<UAnimationStateMachine<TState>>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}