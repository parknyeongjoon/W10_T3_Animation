#pragma once
#include "MovementComponent.h"

struct FRotatingMovementComponentInfo : public FMovementComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FRotatingMovementComponentInfo);

    FRotatingMovementComponentInfo()
        : FMovementComponentInfo()
    {
        InfoType = TEXT("FRotatingMovementComponentInfo");
        ComponentClass = TEXT("URotatingMovementComponent");
    }

    FVector RotationRate;

    virtual void Serialize(FArchive& ar) const override
    {
        FMovementComponentInfo::Serialize(ar);
        ar << RotationRate;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FMovementComponentInfo::Deserialize(ar);
        ar >> RotationRate;
    }
};

class URotatingMovementComponent : public UMovementComponent
{
    DECLARE_CLASS(URotatingMovementComponent, UMovementComponent)
    
public:
    URotatingMovementComponent();
    URotatingMovementComponent(const URotatingMovementComponent& other);
    ~URotatingMovementComponent() override = default;

    /**
     * How fast to update roll/pitch/yaw of UpdateComponent 
     */
    FVector RotationRate;

    virtual void TickComponent(float DeltaTime) override;
    
    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
    
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;

public:
    void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    void LoadAndConstruct(const FActorComponentInfo& Info) override;
};
