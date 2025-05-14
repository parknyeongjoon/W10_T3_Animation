#include "PlayerInput.h"
#include "PlayerController.h"
#include "Components/InputComponent.h"

void UPlayerInput::InitializeDefaultMappings()
{
    // 기본 축 매핑 초기화
    AddAxisMapping({ "MoveForward", EKeys::W,  1.f });
    AddAxisMapping({ "MoveForward", EKeys::S, -1.f });
    AddAxisMapping({ "MoveRight",   EKeys::D,  1.f });
    AddAxisMapping({ "MoveRight",   EKeys::A, -1.f });
    AddAxisMapping({ "Turn",       EKeys::MouseX, 1.f });
    AddAxisMapping({ "LookUp",     EKeys::MouseY, 1.f });
    AddActionMapping({"AnimA", EKeys::Z });
    AddActionMapping({"AnimB", EKeys::X });
    AddActionMapping({"AnimC", EKeys::C });
}

void UPlayerInput::AddActionMapping(const FInputActionKeyMapping& Mapping)
{
    ActionMappings.AddUnique(Mapping);
}

void UPlayerInput::AddAxisMapping(const FInputAxisKeyMapping& Mapping)
{
    AxisMappings.AddUnique(Mapping);
}

void UPlayerInput::RemoveActionMapping(const FString ActionName, const EKeys::Type Key)
{
    ActionMappings.RemoveAll(
        [&](auto& M) { return M.ActionName == ActionName && M.Key == Key; });
}

void UPlayerInput::RemoveAxisMapping(const FString AxisName, const EKeys::Type Key)
{
    AxisMappings.RemoveAll(
        [&](auto& M) { return M.AxisName == AxisName && M.Key == Key; });
}

bool UPlayerInput::IsPressed(EKeys::Type Key) const
{
    return PressedKeys.Contains(Key);
}

float UPlayerInput::GetAxisValue(FString AxisName) const
{
    return 0.0f;
}

void UPlayerInput::InputKey(EKeys::Type Key, EInputEvent EventType)
{
    // Pressed/Released 상태 업데이트
    if (EventType == IE_Pressed)
        PressedKeys.Add(Key);
    else if (EventType == IE_Released)
    {
        PressedKeys.Remove(Key);
        return;
    }

    // 매핑된 액션 찾기
    for (const auto& M : ActionMappings)
    {
        if (M.Key == Key)
        {
            // (필요 시 Shift/Ctrl/Alt 검사 추가)
            // PlayerController 에서 관리하는 InputComponent 스택에 전달
            if (APlayerController* PC = Cast<APlayerController>(GetOuter()))
            {
                for (UInputComponent* IC : PC->GetInputComponentStack())
                {
                    IC->InputKey(M.ActionName);
                }
            }
        }
    }
}

void UPlayerInput::InputAxis(EKeys::Type Key, EInputEvent EventType)
{
    // 매핑된 축 찾기
    if (APlayerController* PC = Cast<APlayerController>(GetOuter()))
    {
        for (const EKeys::Type& Key : PressedKeys)
        {
            for (const auto& Mapping : AxisMappings)
            {
                if (Mapping.Key == Key)
                {
                    float ScaledValue = Mapping.Scale;

                    for (UInputComponent* IC : PC->GetInputComponentStack())
                    {
                        IC->InputAxis(Mapping.AxisName, ScaledValue);
                    }
                }
            }
        }
    }
}

void UPlayerInput::MouseInput(float DeltaX, float DeltaY)
{
    // 마우스 이동 처리
    for (const auto& M : AxisMappings)
    {
        if (M.Key == EKeys::MouseX)
        {
            // PlayerController 에서 관리하는 InputComponent 스택에 전달
            if (APlayerController* PC = Cast<APlayerController>(GetOuter()))
            {
                for (UInputComponent* IC : PC->GetInputComponentStack())
                {
                    IC->InputAxis(M.AxisName, DeltaX);
                }
            }
        }
        else if (M.Key == EKeys::MouseY)
        {
            // PlayerController 에서 관리하는 InputComponent 스택에 전달
            if (APlayerController* PC = Cast<APlayerController>(GetOuter()))
            {
                for (UInputComponent* IC : PC->GetInputComponentStack())
                {
                    IC->InputAxis(M.AxisName, DeltaY);
                }
            }
        }
    }
}

void UPlayerInput::AddActionMappingsTo(TArray<FInputActionKeyMapping>& Out) const
{
    Out = ActionMappings;
}

void UPlayerInput::AddAxisMappingsTo(TArray<FInputAxisKeyMapping>& Out) const
{
    Out = AxisMappings;
}

void UPlayerInput::FlushPressedKeys()
{
}