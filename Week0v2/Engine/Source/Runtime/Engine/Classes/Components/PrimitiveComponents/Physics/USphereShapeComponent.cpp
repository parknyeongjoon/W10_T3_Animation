#include "USphereShapeComponent.h"

USphereShapeComponent::USphereShapeComponent()
{
}

USphereShapeComponent::USphereShapeComponent(const USphereShapeComponent& Other)
{
}

USphereShapeComponent::~USphereShapeComponent()
{
}

void USphereShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

    Radius = 1.0f; // Default radius

    UpateBroadAABB();
}

void USphereShapeComponent::TickComponent(float DeltaTime)
{
}

void USphereShapeComponent::UpdateBroadAABB()
{
    FVector Center = GetComponentLocation();
    FVector Scale = GetComponentScale();
    float Radius = GetRadius() * Scale.x; // Assuming uniform scaling

    FVector Min = Center - FVector(Radius, Radius, Radius);
    FVector Max = Center + FVector(Radius, Radius, Radius);

    BroadAABB = FBoundingBox(Center - FVector(Radius, Radius, Radius), Center + FVector(Radius, Radius, Radius));
}

bool USphereShapeComponent::TestOverlaps(const UShapeComponent* OtherShape) const
{
    if (!BroadPhaseCollisionCheck(OtherShape))
    {
        return false;
    }

    return NarrowPhaseCollisionCheck(OtherShape);
}

bool USphereShapeComponent::NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    return false;
}
