#pragma once
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"

class AGBullet :public AStaticMeshActor
{
    DECLARE_CLASS(AGBullet, AStaticMeshActor)

public:
    AGBullet();
    AGBullet(const AGBullet& Other);
    virtual ~AGBullet() override;

    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    void Destroyed() override;

    void Fire(FVector FirePosition, FVector FireDirection, float Power);
    void ReturnToPool();
    void OnHit(const UPrimitiveComponent* Other);

public:
    FVector Position;
    FVector Velocity;
    FVector Acceleration;
    bool bFired = false;
    float DragCoefficient;
    float LifeTime = 10;
    float LifeElapsed = 0;

public:
    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
};

