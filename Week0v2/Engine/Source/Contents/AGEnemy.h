#pragma once
#include "Components/PrimitiveComponents/UBillboardComponent.h"
#include "Components/PrimitiveComponents/Physics/UCapsuleShapeComponent.h"
#include "Engine/StaticMeshActor.h"

class AGEnemy : public AActor
{
    DECLARE_CLASS(AGEnemy, AActor)
    DECLARE_MULTICAST_DELEGATE(OnHitEvent)
    DECLARE_MULTICAST_DELEGATE(OnDeadEvent)
public:
    AGEnemy();
    AGEnemy(const AGEnemy& Other);
    virtual ~AGEnemy() override = default;
    
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    
    void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;

    void OnDamaged();
    void ChangeColor(FVector NewColor) const;
    void Move();
    void OnCollision(const UPrimitiveComponent* Other);

    OnHitEvent OnHit;
    OnDeadEvent OnDead;
private:
    FVector Velocity; 

    UStaticMeshComponent* MeshComp;
    UCapsuleShapeComponent* Capsule;
    UBillboardComponent* HeartUI[3];

    int health = 3;
};
