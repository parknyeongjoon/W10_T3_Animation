#include "PlayerController.h"
#include "Character.h"
#include "PlayerCameraManager.h"
#include "PlayerInput.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

APlayerController::APlayerController(const APlayerController& other)
{
}

void APlayerController::BeginPlay()
{
    AController::BeginPlay();

    if (!PlayerInput)
    {
        PlayerInput = FObjectFactory::ConstructObject<UPlayerInput>(this);
    }
    PlayerInput->InitializeDefaultMappings();

    if (!PlayerCameraManager)
    {
        PlayerCameraManager = GetWorld()->SpawnActor<APlayerCameraManager>();
        PlayerCameraManager->Initialize(this);
    }
}

void APlayerController::Tick(float DeltaTime)
{
    AController::Tick(DeltaTime);

    // 입력 처리
    if (PlayerInput)
    {
        for (const auto& Key : PlayerInput->PressedKeys)
        {
            // 키에 대응하는 InputAxis 호출
            InputAxis(Key, IE_Axis);
        }
    }
}

void APlayerController::Destroyed()
{
    AController::Destroyed();
}

void APlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AController::EndPlay(EndPlayReason);
}

UObject* APlayerController::Duplicate(UObject* InOuter)
{
    APlayerController* ClonedActor = FObjectFactory::ConstructObjectFrom<APlayerController>(this, InOuter);
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void APlayerController::UpdateCameraManager(float DeltaTime)
{
    if (PlayerCameraManager)
    {
        PlayerCameraManager->UpdateCamera(DeltaTime);
    }
}

void APlayerController::SetupInputComponent()
{
    AController::SetupInputComponent();
    SetupInputBindings();
}

void APlayerController::AddYawInput(float Value)
{
    if (Value < KINDA_SMALL_NUMBER)
        return;

    // Z축(Yaw) 회전만 조절
    FRotator NewRot = GetControlRotation();
    NewRot.Yaw += Value;
    SetControlRotation(NewRot);
}

void APlayerController::AddPitchInput(float Value)
{
    if (Value < KINDA_SMALL_NUMBER)
        return;

    // X축(Pitch) 회전만 조절
    FRotator NewRot = GetControlRotation();
    NewRot.Pitch += Value;
    SetControlRotation(NewRot);
}

bool APlayerController::IsInputKeyDown(EKeys::Type Key) const
{
    if (PlayerInput)
    {
    // 키 상태를 PlayerInput에서 확인
    return PlayerInput->IsPressed(Key);
    }
    return false;
}

void APlayerController::PushInputComponent(UInputComponent* InputComponent)
{
    if (InputComponent)
    {
        // InputComponent를 스택에 추가
        InputComponentStack.Add(InputComponent);
    }
}

void APlayerController::PopInputComponent(UInputComponent* InputComponent)
{
    if (InputComponent)
    {
        // InputComponent를 스택에서 제거
        InputComponentStack.Remove(InputComponent);
    }
}

void APlayerController::InputKey(EKeys::Type Key, EInputEvent EventType)
{
    PlayerInput->InputKey(Key, EventType);
}

void APlayerController::InputAxis(EKeys::Type Key, EInputEvent EventType)
{
    PlayerInput->InputAxis(Key, EventType);
}

void APlayerController::MouseInput(float DeltaX, float DeltaY)
{
    PlayerInput->MouseInput(DeltaX, DeltaY);
}

void APlayerController::SetupInputBindings()
{
    // 카메라 조작용 축 바인딩
    if (InputComponent)
    {
        InputComponent->BindAxis("Turn", [this](float V) { AddYawInput(V); });
        InputComponent->BindAxis("LookUp", [this](float V) { AddPitchInput(V); });
    }
}

ACharacter* APlayerController::GetCharacter() const
{
    return Cast<ACharacter>(GetPawn());
}
