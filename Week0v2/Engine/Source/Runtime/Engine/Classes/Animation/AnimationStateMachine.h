#pragma once
#include "UObject/Object.h"
#include "Container/Array.h"
#include "Container/Map.h"

class UAnimationStateMachine :
    public UObject
{
public:
    void AddState(const FString& StateName, std::function<void(float)> OnUpdate);

    // 상태 전환 정의
    void AddTransition(const FString& FromState, const FString& ToState, std::function<bool()> Condition);

    // 현재 상태를 변경
    void SetState(const FString& NewState);

    // 매 프레임 업데이트
    void Update(float DeltaTime);

    // 현재 상태 확인
    FString GetCurrentState() const;
private:
    FString CurrentState;
    TMap<FString, std::function<void(float)>> States;
    TMap<TPair<FString, FString>, std::function<bool()>> Transitions;
};

