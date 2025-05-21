#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "InputCore/InputCoreTypes.h"

struct FInputAxisKeyMapping
{
    FString AxisName;
    EKeys::Type Key;
    float Scale = 1.f;

    // == 연산자 추가
    bool operator==(const FInputAxisKeyMapping& Other) const
    {
        // '같다'고 판단할 기준을 정확히 명시
        return AxisName == Other.AxisName
            && Key == Other.Key;
    }
};

struct FInputActionKeyMapping
{
    FString ActionName;
    EKeys::Type Key;

    // == 연산자 추가
    bool operator==(const FInputActionKeyMapping& Other) const
    {
        // '같다'고 판단할 기준을 정확히 명시
        return ActionName == Other.ActionName
            && Key == Other.Key;
    }
};

class UPlayerInput : public UObject
{
    DECLARE_CLASS(UPlayerInput, UObject)

public:
    UPlayerInput() = default;

    void InitializeDefaultMappings();
    // 런타임에 매핑 추가/제거
    void AddActionMapping(const FInputActionKeyMapping& Mapping);
    void AddAxisMapping(const FInputAxisKeyMapping& Mapping);
    void RemoveActionMapping(const FString ActionName, const EKeys::Type Key);
    void RemoveAxisMapping(const FString AxisName, const EKeys::Type Key);

    /** 키가 눌려있는지 확인 */
    bool IsPressed(EKeys::Type Key) const;

    /** 축 값 읽기 */
    float GetAxisValue(FString AxisName) const;

    /** 키 이벤트 수신 */
    void InputKey(EKeys::Type Key, EInputEvent EventType);

    /** 축 이벤트 수신 */
    void InputAxis(EKeys::Type Key, EInputEvent EventType);

    void MouseInput(float DeltaX, float DeltaY);

    // SetupInputComponent 시, 매핑 데이터를 InputComponent 에 복사
    void AddActionMappingsTo(TArray<FInputActionKeyMapping>& Out) const;
    void AddAxisMappingsTo(TArray<FInputAxisKeyMapping>& Out) const;
    
    // 현재 눌려 있는 키 집합
    TSet<EKeys::Type> PressedKeys;
protected:
    /** 매 프레임 입력 상태 초기화 */
    void FlushPressedKeys();

private:
    TArray<FInputActionKeyMapping> ActionMappings;
    TArray<FInputAxisKeyMapping> AxisMappings;
};