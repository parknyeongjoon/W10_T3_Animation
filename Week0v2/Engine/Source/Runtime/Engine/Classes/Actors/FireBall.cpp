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

UObject* AFireBall::Duplicate(UObject* InOuter)
{
    AFireBall* NewActor = FObjectFactory::ConstructObjectFrom<AFireBall>(this, InOuter);
    NewActor->DuplicateSubObjects(this, InOuter);
    NewActor->PostDuplicate();
    return NewActor;
}

void AFireBall::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
    AFireBall* SourceActor = Cast<AFireBall>(Source);

    FireBallComponent = Cast<UFireBallComponent>(SourceActor->FireBallComponent->Duplicate(InOuter));
    LightComp = Cast<UPointLightComponent>(SourceActor->LightComp->Duplicate(InOuter));
    ProjMovementComp = Cast<UProjectileMovementComponent>(SourceActor->ProjMovementComp->Duplicate(InOuter));
    BillboardComponent = Cast<UBillboardComponent>(SourceActor->BillboardComponent->Duplicate(InOuter));
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
