#pragma once
#include "Controller.h"
#include "InputCore/InputCoreTypes.h"

class UPlayerInput;
class APlayerCameraManager;
class ACharacter;
class APlayerController : public AController
{
    DECLARE_CLASS(APlayerController, AController)
public:
    APlayerController() = default;
    APlayerController(const APlayerController& other);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UObject* Duplicate(UObject* InOuter) override;

    virtual void UpdateCameraManager(float DeltaTime);

    ACharacter* GetCharacter() const;
    void SetupInputComponent() override;
    // === 입력 기능 ===
    void AddYawInput(float Value);
    void AddPitchInput(float Value);
    bool IsInputKeyDown(EKeys::Type Key) const;
    
    void PushInputComponent(UInputComponent* InputComponent);
    void PopInputComponent(UInputComponent* InputComponent);
    const TArray<UInputComponent*>& GetInputComponentStack() const { return InputComponentStack; }

    void InputKey(EKeys::Type Key, EInputEvent EventType);
    void InputAxis(EKeys::Type Key, EInputEvent EventType);
    void MouseInput(float DeltaX, float DeltaY);

    APlayerCameraManager* GetPlayerCameraManager() const { return PlayerCameraManager; }
    void SetPlayerCameraManager(APlayerCameraManager* InPlayerCameraManager);

protected:
    void SetupInputBindings();
    
    APlayerCameraManager* PlayerCameraManager = nullptr;
    TArray<UInputComponent*> InputComponentStack;
    UPlayerInput* PlayerInput = nullptr;
};
