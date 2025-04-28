#include "AGEnemy.h"

#include "Components/Material/Material.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"
#include "Engine/FLoaderOBJ.h"

AGEnemy::AGEnemy()
{
    MeshComp = AddComponent<UStaticMeshComponent>(EComponentOrigin::Constructor);
    FManagerOBJ::CreateStaticMesh("Assets/Primitives/Capsule.obj");
    MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Capsule.obj"));
    
    Capsule = AddComponent<UCapsuleShapeComponent>(EComponentOrigin::Constructor);
    
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
        HeartUI[i]->SetRelativeLocation(FVector(-1 + i, 0, 1.5f));
    }
    
    ChangeColor(FVector(0,200,100));
}

AGEnemy::AGEnemy(const AGEnemy& Other)
{
}

void AGEnemy::BeginPlay()
{
    Super::BeginPlay();
    OnHit.AddUObject(this, &AGEnemy::OnDamaged);
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
    int heartIndex = 0;
    Super::DuplicateSubObjects(Source);
    for (auto& Comp : GetComponents())
    {
        if (UStaticMeshComponent* SMComp = Cast<UStaticMeshComponent>(Comp))
        {
            MeshComp = SMComp;
        }
        if (UBillboardComponent* HBComp = Cast<UBillboardComponent>(Comp))
        {
            HeartUI[heartIndex] = HBComp;
            HeartUI[heartIndex]->SetRelativeLocation(FVector(-1 + heartIndex, 0, 0));
            heartIndex++;
        }
        if (UCapsuleShapeComponent* CapComp = Cast<UCapsuleShapeComponent>(Comp))
        {
            Capsule = CapComp;
        }
    }
}

void AGEnemy::PostDuplicate()
{
    Super::PostDuplicate();
}

void AGEnemy::OnDamaged()
{
    health--;
    
    switch (health)
    {
    case 2:
        ChangeColor(FVector(255,255,0));
        HeartUI[2]->DestroyComponent();
        break;
    case 1:
        ChangeColor(FVector(255,0,0));
        HeartUI[1]->DestroyComponent();
        break;
    case 0:
        ChangeColor(FVector(0.2,0.2,0.2));
        HeartUI[0]->DestroyComponent();
        break;
    }
}

void AGEnemy::ChangeColor(FVector NewColor) const
{
    MeshComp->GetMaterial(0)->SetDiffuse(NewColor);
}
