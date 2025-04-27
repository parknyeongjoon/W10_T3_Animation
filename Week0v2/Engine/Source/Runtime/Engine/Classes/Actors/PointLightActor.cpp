#include "PointLightActor.h"

#include "Components/LightComponents/PointLightComponent.h"
#include "Components/PrimitiveComponents/UBillboardComponent.h"


APointLightActor::APointLightActor()
{
    LightComponent = AddComponent<UPointLightComponent>(EComponentOrigin::Constructor);
    BillboardComponent->SetTexture(L"Assets/Texture/S_LightPoint.png");
}

APointLightActor::APointLightActor(const APointLightActor& Other)
    : Super(Other)
{
}

void APointLightActor::BeginPlay()
{
    Super::BeginPlay();
}

void APointLightActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APointLightActor::Destroyed()
{
    Super::Destroyed();
}

void APointLightActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool APointLightActor::Destroy()
{
    return Super::Destroy();
}

UObject* APointLightActor::Duplicate() const
{
    APointLightActor* NewActor = FObjectFactory::ConstructObjectFrom<APointLightActor>(this);
    NewActor->DuplicateSubObjects(this);
    NewActor->PostDuplicate();
    return NewActor;
}

void APointLightActor::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void APointLightActor::PostDuplicate()
{
    Super::PostDuplicate();
}

void APointLightActor::LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray)
{
    Super::LoadAndConstruct(InfoArray);
}

FActorInfo APointLightActor::GetActorInfo()
{
    return Super::GetActorInfo();
}
