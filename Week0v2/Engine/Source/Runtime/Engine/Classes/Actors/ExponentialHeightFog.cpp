#include "ExponentialHeightFog.h"

AExponentialHeightFog::AExponentialHeightFog()
    :Super()
{
    FogComponent = AddComponent<UHeightFogComponent>();
}

AExponentialHeightFog::AExponentialHeightFog(const AExponentialHeightFog& Other)
    :Super(Other)
{
}

AExponentialHeightFog::~AExponentialHeightFog()
{
}

void AExponentialHeightFog::BeginPlay()
{
    Super::BeginPlay();
}

void AExponentialHeightFog::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AExponentialHeightFog::Destroyed()
{
    Super::Destroyed();
}

void AExponentialHeightFog::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool AExponentialHeightFog::Destroy()
{
    return Super::Destroy();
}

UObject* AExponentialHeightFog::Duplicate() const
{
    AExponentialHeightFog* NewActor = FObjectFactory::ConstructObjectFrom<AExponentialHeightFog>(this);
    NewActor->DuplicateSubObjects(this);
    NewActor->PostDuplicate();
    return NewActor;
}

void AExponentialHeightFog::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    const AExponentialHeightFog* SourceHeightFogActor = FObjectFactory::ConstructObjectFrom<AExponentialHeightFog>(this);
    if (SourceHeightFogActor == nullptr) return;
    FogComponent = Cast<UHeightFogComponent>(SourceHeightFogActor->FogComponent);
}

void AExponentialHeightFog::PostDuplicate()
{
    Super::PostDuplicate();
}

void AExponentialHeightFog::LoadAndConstruct(const TArray<std::shared_ptr<FActorComponentInfo>>& InfoArray)
{
    Super::LoadAndConstruct(InfoArray);
}

FActorInfo AExponentialHeightFog::GetActorInfo()
{
    return Super::GetActorInfo();
}
