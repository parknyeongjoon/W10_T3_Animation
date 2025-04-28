#include "AGEnemy.h"

#include "Components/Material/Material.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"
#include "Engine/FLoaderOBJ.h"

AGEnemy::AGEnemy()
{
    UStaticMeshComponent* MeshComp = GetStaticMeshComponent();
    FManagerOBJ::CreateStaticMesh("Assets/Primitives/Capsule.obj");
    MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Capsule.obj"));
    Capsule = AddComponent<UCapsuleShapeComponent>(EComponentOrigin::Constructor);
    ChangeColor(FVector(0,200,100));
    for (int i=0;i<3;i++)
    {
        HeartUI[i] = AddComponent<UBillboardComponent>(EComponentOrigin::Constructor);
        if (USceneComponent* ParentComponent = Cast<USceneComponent>(MeshComp))
        {
            HeartUI[i]->DetachFromParent();
            HeartUI[i]->SetupAttachment(ParentComponent);
        }
        HeartUI[i]->SetTexture(L"Assets/Texture/heartpixelart.png");
        HeartUI[i]->SetRelativeLocation(FVector(0.0f, 0.0f, 3.0f));
        HeartUI[i]->bOnlyForEditor = false;
        HeartUI[i]->SetRelativeLocation(FVector(0, -1.0f + i, 1.5f));
    }
}

AGEnemy::AGEnemy(const AGEnemy& Other)
{
    
}

void AGEnemy::BeginPlay()
{
    Super::BeginPlay();
    // FCoroutineManager::StartCoroutine()
}

void AGEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

UObject* AGEnemy::Duplicate() const
{
    AGEnemy* NewActor = FObjectFactory::ConstructObjectFrom<AGEnemy>(this);
    NewActor->DuplicateSubObjects(this);
    return NewActor;
}

void AGEnemy::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void AGEnemy::PostDuplicate()
{
    Super::PostDuplicate();
}

void AGEnemy::OnDamaged()
{
    switch (health)
    {
    case 2:
        ChangeColor(FVector(255,255,0));
        break;
    case 1:
        ChangeColor(FVector(255,0,0));
        break;
    case 0:
        ChangeColor(FVector(30,30,30));
        break;
    }
}

void AGEnemy::ChangeColor(FVector NewColor) const
{
    GetStaticMeshComponent()->GetMaterial(0)->SetDiffuse(NewColor);
}
