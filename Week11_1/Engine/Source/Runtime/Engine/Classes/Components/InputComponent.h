#pragma once
#include "Delegates/Delegate.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UInputComponent : public UObject
{
    DECLARE_CLASS(UInputComponent, UObject)
public:
    UInputComponent() = default;

    // 실제 바인딩된 델리게이트
    TMap<FString, TMulticastDelegate<void()>>       ActionBindings;
    TMap<FString, TMulticastDelegate<void(float)>>  AxisBindings;

    // 바인딩 함수들
    void BindAction(const FString& ActionName, std::function<void()> Func);
    void BindAxis(const FString& AxisName, std::function<void(float)> Func);

    void InputKey(const FString& ActionName);
    void InputAxis(const FString& AxisName, float Value);
};
