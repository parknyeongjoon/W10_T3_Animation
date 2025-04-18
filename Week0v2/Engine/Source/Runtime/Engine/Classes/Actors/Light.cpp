#include "Light.h"
#include "Components/PrimitiveComponents/UBillboardComponent.h"

ALight::ALight()
    : Super()
{
    BillboardComponent = AddComponent<UBillboardComponent>();
}

ALight::ALight(const ALight& Other)
    : Super(Other)
{
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

UObject* ALight::Duplicate() const
{
    ALight* NewActor = FObjectFactory::ConstructObjectFrom<ALight>(this);
    NewActor->DuplicateSubObjects(this);
    NewActor->PostDuplicate();
    return NewActor;
}

void ALight::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    const ALight* SourceLightActor = FObjectFactory::ConstructObjectFrom<ALight>(this);
    if (SourceLightActor == nullptr) return;

    LightComponent = Cast<ULightComponentBase>(SourceLightActor->LightComponent);
    BillboardComponent = Cast<UBillboardComponent>(SourceLightActor->BillboardComponent);
}

void ALight::PostDuplicate()
{
    Super::PostDuplicate();
}

void ALight::LoadAndConstruct(const TArray<std::shared_ptr<FActorComponentInfo>>& InfoArray)
{
    Super::LoadAndConstruct(InfoArray);
}

FActorInfo ALight::GetActorInfo()
{
    return Super::GetActorInfo();
}

