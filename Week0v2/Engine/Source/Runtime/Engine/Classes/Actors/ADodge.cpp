#include "ADodge.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"
#include "Engine/FLoaderOBJ.h"
#include "Engine/World.h"

ADodge::ADodge()
    : Super()
{
    // SetActorLabel(TEXT("OBJ_DODGE"));
    UStaticMeshComponent* MeshComp = GetStaticMeshComponent();
    FManagerOBJ::CreateStaticMesh("Assets/Dodge/Dodge.obj");
    MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Dodge.obj"));
}

ADodge::ADodge(const ADodge& Other)
    : Super(Other)
{
}

void ADodge::BeginPlay()
{
    Super::BeginPlay();
    TestDelegate.AddUObject(this, &ADodge::test);
    TestDelegate.AddLambda([this]{SetActorLocation(GetActorLocation() + FVector(0.1,0,0));});
}

void ADodge::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TestDelegate.Broadcast();
}

void ADodge::Destroyed()
{
    Super::Destroyed();
}

void ADodge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool ADodge::Destroy()
{
    return Super::Destroy();
}

void ADodge::test()
{
    SetActorRotation(GetActorRotation() + FVector(0.01,0,0));
    // Destroy();
}

UObject* ADodge::Duplicate() const
{
    ADodge* NewActor = FObjectFactory::ConstructObjectFrom<ADodge>(this);
    NewActor->DuplicateSubObjects(this);
    NewActor->PostDuplicate();
    return NewActor;
}

void ADodge::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void ADodge::PostDuplicate()
{
    Super::PostDuplicate();
    // TestDelegate = TestDelegate.DuplicateDelegate<>(GetLevel());
}

void ADodge::LoadAndConstruct(const TArray<std::shared_ptr<FActorComponentInfo>>& InfoArray)
{
    AActor::LoadAndConstruct(InfoArray);
}

FActorInfo ADodge::GetActorInfo()
{
    return AActor::GetActorInfo();
}
