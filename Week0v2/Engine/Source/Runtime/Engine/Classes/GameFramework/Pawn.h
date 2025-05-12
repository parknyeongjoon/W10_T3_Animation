#pragma once
#include "Actor.h"

class UInputComponent;
class APlayerController;
class AController;

class APawn : public AActor
{
    DECLARE_CLASS(APawn, AActor)
public:
    APawn() = default;
    APawn(const APawn& other);
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UObject* Duplicate(UObject* InOuter) override;

    /** Pawn을 Controller에 의해 점유(Possess)될 때 호출 */
    virtual void PossessedBy(AController* NewController);

    /** Pawn이 Controller에서 해제(UnPossess)될 때 호출 */
    virtual void UnPossessed();

    /** 플레이어 입력을 수신하고 처리 */
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

    /** 입력 처리용 함수 */
    virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f);

    /** 입력을 받아 회전 처리 */
    virtual void AddControllerYawInput(float Value);
    virtual void AddControllerPitchInput(float Value);

    /** 시야 관련 함수 */
    virtual FVector GetPawnViewLocation() const;
    virtual FRotator GetViewRotation() const;

    virtual void EnableInput(APlayerController* PlayerController);
    virtual void DisableInput(APlayerController* PlayerController);

    AController* GetController() const { return Controller; }

protected:
    UInputComponent* InputComponent;
    FVector PendingMovement;
    AController* Controller = nullptr;
    float MoveSpeed;
};
