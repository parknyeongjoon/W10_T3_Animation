#include "InputComponent.h"

void UInputComponent::BindAction(const FString& ActionName, std::function<void()> Func)
{
    ActionBindings.FindOrAdd(ActionName).AddLambda(std::move(Func));
}

void UInputComponent::BindAxis(const FString& AxisName, std::function<void(float)> Func)
{
    AxisBindings.FindOrAdd(AxisName).AddLambda(std::move(Func));
}

void UInputComponent::InputKey(const FString& ActionName)
{
    if (TMulticastDelegate<void()>* Delegate = ActionBindings.Find(ActionName))
    {
        Delegate->Broadcast();
    }
}

void UInputComponent::InputAxis(const FString& AxisName, float Value)
{
    if (TMulticastDelegate<void(float)>* Delegate = AxisBindings.Find(AxisName))
    {
        Delegate->Broadcast(Value);
    }
}