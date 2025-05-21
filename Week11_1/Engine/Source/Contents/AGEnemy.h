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
    virtual ~AGEnemy() override = default;
    
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    
    void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    UObject* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    void PostDuplicate() override;

    void OnDamaged();
    void ChangeColor(FVector NewColor) const;
    void Move();
    void OnCollision(const UPrimitiveComponent* Other);

    OnHitEvent OnHit;
    OnDeadEvent OnDead;
private:
    FVector Velocity;
    bool bIsDead= false;
    float DeadTimer = 0.0f;

    UStaticMeshComponent* MeshComp;
    UCapsuleShapeComponent* Capsule;
    UBillboardComponent* HeartUI[3];

    int health = 3;
};
