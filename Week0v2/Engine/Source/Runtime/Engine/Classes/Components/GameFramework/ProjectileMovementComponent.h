#pragma once
#include "MovementComponent.h"

class UProjectileMovementComponent : public UMovementComponent
{
    DECLARE_CLASS(UProjectileMovementComponent, UMovementComponent)
    
public:
    UProjectileMovementComponent();
    UProjectileMovementComponent(const UProjectileMovementComponent& Other);

    void TickComponent(float DeltaTime) override;

    void InitializeComponent() override;
    
    virtual void SetVelocityInLocalSpace(FVector NewVelocity);

    virtual FVector CalculateVelocity(FVector OldVelocity, float DeltaTime);

    float GetMaxSpeed() const override { return MaxSpeed; }

    bool ShouldApplyGravity() const { return ProjectileGravityScale != 0.f; }

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
    
public:
    float InitialSpeed;

    float MaxSpeed;

    bool bRotationFollowsVelocity;

    bool bInitialVelocityInLocalSpace;

    float ProjectileGravityScale;

protected:
    FVector LimitVelocity(FVector NewVelocity) const;

    virtual FVector ComputeMoveDelta(const FVector& InVelocity, float DeltaTime, bool bApplyGravity = true) const;

    float GetEffectiveGravityZ() const;
};
