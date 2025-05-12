#include "Character.h"

ACharacter::ACharacter(const ACharacter& Other)
{
}

void ACharacter::BeginPlay()
{
    APawn::BeginPlay();
}

void ACharacter::Tick(float DeltaTime)
{
    APawn::Tick(DeltaTime);
}

void ACharacter::Destroyed()
{
    APawn::Destroyed();
}

void ACharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    APawn::EndPlay(EndPlayReason);
}

UObject* ACharacter::Duplicate(UObject* InOuter)
{
    ACharacter* ClonedActor = FObjectFactory::ConstructObjectFrom<ACharacter>(this, InOuter);
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void ACharacter::PossessedBy(AController* NewController)
{
    APawn::PossessedBy(NewController);
}

void ACharacter::UnPossessed()
{
    APawn::UnPossessed();
}
