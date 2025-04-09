#pragma once
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"

struct FMovementComponentInfo : public FActorComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FMovementComponentInfo)
    FMovementComponentInfo()
        : FActorComponentInfo()
    {
        InfoType = TEXT("FMovementComponentInfo");
        ComponentType = TEXT("UMovementComponent");
    }

    bool bUpdateOnlyIfRendered;
    bool bAutoUpdateTickRegistration;
    bool bAutoRegisterUpdatedComponent;
    FVector Velocity;

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FActorComponentInfo::Copy(Other);
        FMovementComponentInfo& MovementInfo = static_cast<FMovementComponentInfo&>(Other);
        MovementInfo.bUpdateOnlyIfRendered = bUpdateOnlyIfRendered;
        MovementInfo.bAutoUpdateTickRegistration = bAutoUpdateTickRegistration;
        MovementInfo.bAutoRegisterUpdatedComponent = bAutoRegisterUpdatedComponent;
        MovementInfo.Velocity = Velocity;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FActorComponentInfo::Serialize(ar);
        ar << bUpdateOnlyIfRendered;
        ar << bAutoUpdateTickRegistration;
        ar << bAutoRegisterUpdatedComponent;
        ar << Velocity;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FActorComponentInfo::Deserialize(ar);
        ar >> bUpdateOnlyIfRendered;
        ar >> bAutoUpdateTickRegistration;
        ar >> bAutoRegisterUpdatedComponent;
        ar >> Velocity;
    }
};

class UMovementComponent : public UActorComponent
{
    DECLARE_CLASS(UMovementComponent, UActorComponent)

    /** The component we move and update. */
    class UPrimitiveComponent* UpdatedComponent;

    /** Current Velocity of moved component. */
    FVector Velocity;

public:
    UMovementComponent();

    UMovementComponent(const UMovementComponent& Other);
    
public:
    /** If true, skips TickComponent() if UpdatedComponent was not recently rendered. */
    bool bUpdateOnlyIfRendered;

    /** If true, whenever the updated component is changed*/
    bool bAutoUpdateTickRegistration;

    /** If true, registers the owner's Root component as the UpdatedComponent if there is not one currently assigned. */
    bool bAutoRegisterUpdatedComponent;

    virtual float GetGravityZ() const;

    virtual float GetMaxSpeed() const;

    virtual float GetMaxSpeedModifier() const;

    virtual float GetModifiedMaxSpeed() const;

    virtual void StopMovementImmediately();

    virtual void UpdateComponentVelocity();

    virtual void InitializeComponent() override;

    // 기존 언리얼 코드에서는 USceneComponent의 인터페이스입니다.
    // 언리얼 엔진의 UPrimitiveComponent::MoveComponent 함수를 참고하십시오.
    bool MoveComponent(const FVector& Delta);

    bool bInInitializeComponent;

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

public:
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);

};

inline float UMovementComponent::GetMaxSpeed() const
{
    return 0.f;
}

inline float UMovementComponent::GetMaxSpeedModifier() const
{
    return 1.0f;
}

inline float UMovementComponent::GetModifiedMaxSpeed() const
{
    return GetMaxSpeed() * GetMaxSpeedModifier();
}

inline void UMovementComponent::StopMovementImmediately()
{
    Velocity = FVector::ZeroVector;
}
