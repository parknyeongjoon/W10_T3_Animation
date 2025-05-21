#include "DirectionalLightActor.h"

#include "Components/LightComponents/DirectionalLightComponent.h"
#include "Components/PrimitiveComponents/UBillboardComponent.h"


ADirectionalLightActor::ADirectionalLightActor()
    : Super()
{
    LightComponent = AddComponent<UDirectionalLightComponent>(EComponentOrigin::Constructor);
    BillboardComponent->SetTexture(L"Assets/Texture/S_LightDirectional.png");
}

void ADirectionalLightActor::BeginPlay()
{
    Super::BeginPlay();
}

void ADirectionalLightActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADirectionalLightActor::Destroyed()
{
    Super::Destroyed();
}

void ADirectionalLightActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool ADirectionalLightActor::Destroy()
{
    return Super::Destroy();
}

UObject* ADirectionalLightActor::Duplicate(UObject* InOuter)
{
    ALight* NewActor = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewActor->DuplicateSubObjects(this, InOuter);
    NewActor->PostDuplicate();
    return NewActor;
}

void ADirectionalLightActor::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
}

void ADirectionalLightActor::PostDuplicate()
{
    Super::PostDuplicate();
}

void ADirectionalLightActor::LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray)
{
    Super::LoadAndConstruct(InfoArray);
}

FActorInfo ADirectionalLightActor::GetActorInfo()
{
    return Super::GetActorInfo();
}
