#include "FireBallActor.h"

#include "Components/PointLightComponent.h"
#include "Components/GameFramework/ProjectileMovementComponent.h"
#include "Components/UBillboardComponent.h"

AFireBallActor::AFireBallActor()
    : Super()
{
    FireBallComponent = AddComponent<UFireBallComponent>();
    LightComp = AddComponent<UPointLightComponent>();
    LightComp->SetIntensity(3.f);
    LightComp->SetRadius(20.f);
    ProjMovementComp = AddComponent<UProjectileMovementComponent>();
    BillboardComponent = AddComponent<UBillboardComponent>();
    BillboardComponent->SetTexture(L"Assets/Texture/spotLight.png");
}

AFireBallActor::AFireBallActor(const AFireBallActor& Other)
    : Super(Other)
{
}

void AFireBallActor::BeginPlay()
{
    Super::BeginPlay();
}

void AFireBallActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFireBallActor::Destroyed()
{
    Super::Destroyed();
}

void AFireBallActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool AFireBallActor::Destroy()
{
    return Super::Destroy();
}

UObject* AFireBallActor::Duplicate() const
{
    AFireBallActor* NewActor = FObjectFactory::ConstructObjectFrom<AFireBallActor>(this);
    NewActor->DuplicateSubObjects(this);
    NewActor->PostDuplicate();
    return NewActor;
}

void AFireBallActor::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    AFireBallActor* SourceActor = Cast<AFireBallActor>(Source);

    FireBallComponent = Cast<UFireBallComponent>(SourceActor->FireBallComponent->Duplicate());
    LightComp = Cast<UPointLightComponent>(SourceActor->LightComp->Duplicate());
    ProjMovementComp = Cast<UProjectileMovementComponent>(SourceActor->ProjMovementComp->Duplicate());
    BillboardComponent = Cast<UBillboardComponent>(SourceActor->BillboardComponent->Duplicate());
}

void AFireBallActor::PostDuplicate()
{
    Super::PostDuplicate();
}

void AFireBallActor::LoadAndConstruct(const TArray<std::shared_ptr<FActorComponentInfo>>& InfoArray)
{
    AActor::LoadAndConstruct(InfoArray);
}

FActorInfo AFireBallActor::GetActorInfo()
{
    return AActor::GetActorInfo();
}
