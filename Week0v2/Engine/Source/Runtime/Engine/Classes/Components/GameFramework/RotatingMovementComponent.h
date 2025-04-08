#pragma once
#include "MovementComponent.h"

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
};
