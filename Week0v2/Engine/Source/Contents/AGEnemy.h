#pragma once
#include "Components/PrimitiveComponents/Physics/UCapsuleShapeComponent.h"
#include "Engine/StaticMeshActor.h"

class AGEnemy : public AStaticMeshActor
{
    DECLARE_CLASS(AGEnemy, AStaticMeshActor)
    DECLARE_MULTICAST_DELEGATE(OnHitEvent)
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
private:
    OnHitEvent OnHit;
    UCapsuleShapeComponent* Capsule;
    AActor* HeartUI[3];

    int health = 3;
};
