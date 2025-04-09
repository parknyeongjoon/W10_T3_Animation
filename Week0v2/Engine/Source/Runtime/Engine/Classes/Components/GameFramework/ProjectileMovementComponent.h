#pragma once
#include "MovementComponent.h"

struct FProjectileMovementComponentInfo : public FMovementComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FProjectileMovementComponentInfo);

    FProjectileMovementComponentInfo()
        : FMovementComponentInfo()
        , InitialSpeed(0.f)
        , MaxSpeed(0.f)
        , bRotationFollowsVelocity(false)
        , bInitialVelocityInLocalSpace(false)
        , ProjectileGravityScale(0.f)
    {
        InfoType = TEXT("FProjectileMovementComponentInfo");
        ComponentType = TEXT("UProjectileMovementComponent");
    }
    float InitialSpeed;
    float MaxSpeed;
    bool bRotationFollowsVelocity;
    bool bInitialVelocityInLocalSpace;
    float ProjectileGravityScale;
    virtual void Copy(FActorComponentInfo& Other) override
    {
        FMovementComponentInfo::Copy(Other);
        FProjectileMovementComponentInfo& ProjectileMovementInfo = static_cast<FProjectileMovementComponentInfo&>(Other);
        ProjectileMovementInfo.InitialSpeed = InitialSpeed;
        ProjectileMovementInfo.MaxSpeed = MaxSpeed;
        ProjectileMovementInfo.bRotationFollowsVelocity = bRotationFollowsVelocity;
        ProjectileMovementInfo.bInitialVelocityInLocalSpace = bInitialVelocityInLocalSpace;
        ProjectileMovementInfo.ProjectileGravityScale = ProjectileGravityScale;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FMovementComponentInfo::Serialize(ar);
        ar << InitialSpeed;
        ar << MaxSpeed;
        ar << bRotationFollowsVelocity;
        ar << bInitialVelocityInLocalSpace;
        ar << ProjectileGravityScale;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FMovementComponentInfo::Deserialize(ar);
        ar >> InitialSpeed;
        ar >> MaxSpeed;
        ar >> bRotationFollowsVelocity;
        ar >> bInitialVelocityInLocalSpace;
        ar >> ProjectileGravityScale;
    }
};

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
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);

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
