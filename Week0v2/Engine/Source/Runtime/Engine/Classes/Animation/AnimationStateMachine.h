#pragma once
#include "UObject/Object.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "Delegates/FFunctor.h"
#include "UObject/Casts.h"

template <typename TState>
class UAnimationStateMachine : public UObject
{
    DECLARE_CLASS(UAnimationStateMachine, UObject)
    using StateCallback = FFunctorWithContext<UAnimInstance, void, float>;
public:
    UAnimationStateMachine() = default;
    UAnimationStateMachine(const UAnimationStateMachine& Other);
    virtual UObject* Duplicate(UObject* InOuter) override;

    void AddState(TState StateName, StateCallback OnUpdate)
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
    
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;

private:
    TState CurrentState;
    TMap<TState, StateCallback> States;
    TMap<TPair<TState, TState>, std::function<bool()>> Transitions;
};

template<typename TState>
inline UAnimationStateMachine<TState>::UAnimationStateMachine(const UAnimationStateMachine& Other)
    :UObject(Other),
    CurrentState(Other.CurrentState),
    States(Other.States),
    Transitions(Other.Transitions)
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

template<typename TState>
inline void UAnimationStateMachine<TState>::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UObject::DuplicateSubObjects(Source, InOuter);
    for (auto& [State, Callback]: States)
    {
        Callback.Bind(Cast<UAnimInstance>(InOuter));
    }
}
