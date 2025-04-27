#include "FireBall.h"

#include "Components/GameFramework/ProjectileMovementComponent.h"
#include "Components/LightComponents/PointLightComponent.h"
#include "Components/PrimitiveComponents/UBillboardComponent.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/FireBallComponent.h"

AFireBall::AFireBall()
    : Super()
{
    FireBallComponent = AddComponent<UFireBallComponent>(EComponentOrigin::Constructor);
    LightComp = AddComponent<UPointLightComponent>(EComponentOrigin::Constructor);
    LightComp->SetIntensity(3.f);
    LightComp->SetRadius(20.f);
    ProjMovementComp = AddComponent<UProjectileMovementComponent>(EComponentOrigin::Constructor);
    BillboardComponent = AddComponent<UBillboardComponent>(EComponentOrigin::Constructor);
    BillboardComponent->SetTexture(L"Assets/Texture/spotLight.png");
}

AFireBall::AFireBall(const AFireBall& Other)
    : Super(Other)
{
}

void AFireBall::BeginPlay()
{
    Super::BeginPlay();
}

void AFireBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFireBall::Destroyed()
{
    Super::Destroyed();
}

void AFireBall::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool AFireBall::Destroy()
{
    return Super::Destroy();
}

UObject* AFireBall::Duplicate() const
{
    AFireBall* NewActor = FObjectFactory::ConstructObjectFrom<AFireBall>(this);
    NewActor->DuplicateSubObjects(this);
    NewActor->PostDuplicate();
    return NewActor;
}

void AFireBall::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    AFireBall* SourceActor = Cast<AFireBall>(Source);

    FireBallComponent = Cast<UFireBallComponent>(SourceActor->FireBallComponent->Duplicate());
    LightComp = Cast<UPointLightComponent>(SourceActor->LightComp->Duplicate());
    ProjMovementComp = Cast<UProjectileMovementComponent>(SourceActor->ProjMovementComp->Duplicate());
    BillboardComponent = Cast<UBillboardComponent>(SourceActor->BillboardComponent->Duplicate());
}

void AFireBall::PostDuplicate()
{
    Super::PostDuplicate();
}

void AFireBall::LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray)
{
    AActor::LoadAndConstruct(InfoArray);
}

FActorInfo AFireBall::GetActorInfo()
{
    return AActor::GetActorInfo();
}
