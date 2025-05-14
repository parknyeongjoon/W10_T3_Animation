#include "Character.h"

#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponents/Physics/UCapsuleShapeComponent.h"
#include "Animation/CustomAnimInstance/TestAnimInstance.h"

ACharacter::ACharacter()
{
    BodyMesh = AddComponent<USkeletalMeshComponent>(EComponentOrigin::Constructor);
    BodyMesh->SetData("Contents/FBX/Rumba_Dancing.fbx");
    UTestAnimInstance* TestAnimInstance = FObjectFactory::ConstructObject<UTestAnimInstance>(BodyMesh);
    BodyMesh->SetAnimInstance(TestAnimInstance);
    RootComponent = BodyMesh;
    
    CollisionCapsule = AddComponent<UCapsuleShapeComponent>(EComponentOrigin::Constructor);
    CollisionCapsule->SetupAttachment(RootComponent);
    
    float XSize = BodyMesh->GetBoundingBox().max.X - BodyMesh->GetBoundingBox().min.X;
    float YSize = BodyMesh->GetBoundingBox().max.Y - BodyMesh->GetBoundingBox().min.Y;
    float ZSize = BodyMesh->GetBoundingBox().max.Z - BodyMesh->GetBoundingBox().min.Z;
    CollisionCapsule->SetRadius(std::min(XSize / 2, YSize / 2));
    CollisionCapsule->SetHalfHeight(ZSize / 2);
    CollisionCapsule->SetRelativeLocation(FVector(0, 0, ZSize/2));
}

ACharacter::ACharacter(const ACharacter& Other)
    : APawn(Other)
{
}

UObject* ACharacter::Duplicate(UObject* InOuter)
{
    ACharacter* ClonedActor = FObjectFactory::ConstructObjectFrom<ACharacter>(this, InOuter);
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void ACharacter::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    APawn::DuplicateSubObjects(Source, InOuter);
    
    ACharacter* Character = Cast<ACharacter>(Source);
    BodyMesh = Cast<USkeletalMeshComponent>(Character->BodyMesh->Duplicate(this));
    CollisionCapsule = Cast<UCapsuleShapeComponent>(Character->CollisionCapsule->Duplicate(this));

    AddDuplicatedComponent(BodyMesh);
    AddDuplicatedComponent(CollisionCapsule);
}

void ACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ACharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
}

void ACharacter::UnPossessed()
{
    Super::UnPossessed();
}
