#include "Light.h"
#include "Components/PrimitiveComponents/UBillboardComponent.h"
#include "Components/LightComponents/LightComponent.h"

ALight::ALight()
    : Super()
{
    BillboardComponent = AddComponent<UBillboardComponent>(EComponentOrigin::Constructor);
}

ALight::~ALight()
{
}

void ALight::BeginPlay()
{
    Super::BeginPlay();
}

void ALight::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

void ALight::Destroyed()
{
    Super::Destroyed();
}

void ALight::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool ALight::Destroy()
{
    return Super::Destroy();
}

UObject* ALight::Duplicate(UObject* InOuter)
{
    ALight* NewActor = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewActor->DuplicateSubObjects(this, InOuter);
    NewActor->PostDuplicate();
    return NewActor;
}

void ALight::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);

    LightComponent = Cast<ULightComponentBase>(LightComponent->Duplicate(LightComponent->GetOuter()));
    BillboardComponent = Cast<UBillboardComponent>(BillboardComponent->Duplicate(BillboardComponent->GetOuter()));
}

void ALight::PostDuplicate()
{
    Super::PostDuplicate();
}

void ALight::LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray)
{
    Super::LoadAndConstruct(InfoArray);
}

FActorInfo ALight::GetActorInfo()
{
    return Super::GetActorInfo();
}

