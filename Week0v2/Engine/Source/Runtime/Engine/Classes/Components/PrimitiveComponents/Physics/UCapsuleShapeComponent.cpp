#include "UCapsuleShapeComponent.h"

UCapsuleShapeComponent::UCapsuleShapeComponent()
{
}

UCapsuleShapeComponent::UCapsuleShapeComponent(const UCapsuleShapeComponent& Other)
{
}

UCapsuleShapeComponent::~UCapsuleShapeComponent()
{
}

void UCapsuleShapeComponent::InitializeComponent()
{
}

void UCapsuleShapeComponent::TickComponent(float DeltaTime)
{
}

bool UCapsuleShapeComponent::TestOverlaps(const UShapeComponent* OtherShape) const
{
    return false;
}

bool UCapsuleShapeComponent::NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    return false;
}
