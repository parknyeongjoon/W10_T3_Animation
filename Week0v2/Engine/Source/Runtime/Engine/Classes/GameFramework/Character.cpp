#include "Character.h"

#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponents/Physics/UCapsuleShapeComponent.h"
#include "Animation/CustomAnimInstance/TestAnimInstance.h"
#include "Components/InputComponent.h"
#include "Components/GameFramework/ProjectileMovementComponent.h"
#include "Contents/GameManager.h"
#include "UObject/FunctionRegistry.h"

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

    MovementComponent = AddComponent<UProjectileMovementComponent>(EComponentOrigin::Constructor);

    PlayAnimA.AddStatic(FGameManager::PlayAnimA);
    PlayAnimB.AddStatic(FGameManager::PlayAnimB);
    PlayAnimC.AddStatic(FGameManager::PlayAnimC);

    FGameManager::Get().GameOverEvent.AddLambda([this]{ Cast<UTestAnimInstance>(BodyMesh->GetAnimInstance())->SetState(ETestState::Defeated); });
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
    ACharacter* Character = Cast<ACharacter>(Source);
    BodyMesh = Cast<USkeletalMeshComponent>(Character->BodyMesh->Duplicate(this));
    RootComponent = BodyMesh;
    CollisionCapsule = Cast<UCapsuleShapeComponent>(Character->CollisionCapsule->Duplicate(this));
    MovementComponent = Cast<UProjectileMovementComponent>(Character->MovementComponent->Duplicate(this));

    AddDuplicatedComponent(BodyMesh);
    AddDuplicatedComponent(CollisionCapsule);
    AddDuplicatedComponent(MovementComponent);
}

void ACharacter::Tick(float DeltaTime) // TODO: Character 상속받는 MyCharacter 만들어서 거기에다가 제작
{
    APawn::Tick(DeltaTime);
    MovementComponent->Velocity *= 0.9f;
    if (MovementComponent->Velocity.Magnitude() < 0.1f)
    {
        MovementComponent->Velocity = FVector::ZeroVector;
    }
}

void ACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) // TODO: Character 상속받는 MyCharacter 만들어서 거기에다가 제작
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // 카메라 조작용 축 바인딩
    if (PlayerInputComponent)
    {
        PlayerInputComponent->BindAxis("MoveForward", [this](float V) { GetMovementComponent()->Velocity += FVector(V,0,0); });
        PlayerInputComponent->BindAxis("MoveForward", [this](float V) { GetMovementComponent()->Velocity += FVector(V,0,0); });
        PlayerInputComponent->BindAxis("MoveRight", [this](float V) { GetMovementComponent()->Velocity += FVector(0,V,0); });
        PlayerInputComponent->BindAxis("MoveRight", [this](float V) { GetMovementComponent()->Velocity += FVector(0,V,0); });
        PlayerInputComponent->BindAction("AnimA", [this] { Cast<UTestAnimInstance>(BodyMesh->GetAnimInstance())->SetState(ETestState::Pose); PlayAnimA.Broadcast(); });
        PlayerInputComponent->BindAction("AnimB", [this] { Cast<UTestAnimInstance>(BodyMesh->GetAnimInstance())->SetState(ETestState::Jump); PlayAnimB.Broadcast(); });
        PlayerInputComponent->BindAction("AnimC", [this] { Cast<UTestAnimInstance>(BodyMesh->GetAnimInstance())->SetState(ETestState::Dance); PlayAnimC.Broadcast(); });
    }
}


void ACharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
}

void ACharacter::UnPossessed()
{
    Super::UnPossessed();
}
