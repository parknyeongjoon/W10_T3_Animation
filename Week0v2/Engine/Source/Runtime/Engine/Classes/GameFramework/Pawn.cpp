#include "Pawn.h"
#include "PlayerController.h"
#include "Components/InputComponent.h"

APawn::APawn(const APawn& other)
    :AActor(other)
{
}

void APawn::BeginPlay()
{
    AActor::BeginPlay();
}

void APawn::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

    if (!PendingMovement.IsNearlyZero())
    {
        // 최대 길이를 1로 정규화한 뒤 속도·델타타임 곱해서 이동량 계산
        FVector Dir = PendingMovement.Normalize();
        FVector Delta = Dir * MoveSpeed * DeltaTime;

        // sweep=true 로 충돌 처리
        //AddActorWorldOffset(Delta, true);

        // 일단 충돌 처리 없이 이동
        SetActorLocation(GetActorLocation() + Delta);

        // 다음 프레임을 위해 초기화
        PendingMovement = FVector::ZeroVector;
    }
}

void APawn::Destroyed()
{
    AActor::Destroyed();
}

void APawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AActor::EndPlay(EndPlayReason);
}

UObject* APawn::Duplicate(UObject* InOuter)
{
    APawn* ClonedActor = FObjectFactory::ConstructObjectFrom<APawn>(this, InOuter);
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void APawn::PossessedBy(AController* NewController)
{
    Controller = NewController;

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        EnableInput(PC);
    }

}

void APawn::UnPossessed()
{
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        DisableInput(PC);
    }

    Controller = nullptr;
}


void APawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
}

void APawn::AddMovementInput(FVector WorldDirection, float ScaleValue)
{
    if (WorldDirection.IsNearlyZero() || ScaleValue == 0.0f)
    {
        return;
    }
    // 이동량을 누적
    PendingMovement += WorldDirection.GetSafeNormal() * ScaleValue;
}

void APawn::AddControllerYawInput(float Value)
{
    if (Value != 0.0f)
    {
        // Yaw 회전 처리
        FRotator NewRotation = GetActorRotation();
        NewRotation.Yaw += Value * 0.2;
        SetActorRotation(NewRotation);
    }
}

void APawn::AddControllerPitchInput(float Value)
{
    if (Value != 0.0f)
    {
        // Pitch 회전 처리
        FRotator NewRotation = GetActorRotation();
        NewRotation.Pitch -= Value * 0.2;
        SetActorRotation(NewRotation);
    }
}

FVector APawn::GetPawnViewLocation() const
{
    return {};
}

FRotator APawn::GetViewRotation() const
{
    return FRotator();
}

void APawn::EnableInput(APlayerController* PlayerController)
{
    if (!InputComponent)
    {
        InputComponent = FObjectFactory::ConstructObject<UInputComponent>(this);
        SetupPlayerInputComponent(InputComponent);
    }

    PlayerController->PushInputComponent(InputComponent);
}

void APawn::DisableInput(APlayerController* PlayerController)
{
    if (!PlayerController || !InputComponent)
    {
        return;
    }

    // 입력 스택에서 제거
    PlayerController->PopInputComponent(InputComponent);
}
