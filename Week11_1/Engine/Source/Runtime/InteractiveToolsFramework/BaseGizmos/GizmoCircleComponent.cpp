#include "GizmoCircleComponent.h"

#include "UnrealEd/EditorViewportClient.h"

#define DISC_RESOLUTION 128

UGizmoCircleComponent::UGizmoCircleComponent()
{
}

UGizmoCircleComponent::~UGizmoCircleComponent()
{
}

bool UGizmoCircleComponent::IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist)
{
    if (rayDir.Y == 0) return false; // normal to normal vector of plane

    dist = -rayOrigin.Y / rayDir.Y;

    FVector intersectionPoint = rayOrigin + rayDir * dist;
    float intersectionToDiscCenterSquared = intersectionPoint.Magnitude();

    return (inner * inner < intersectionToDiscCenterSquared && intersectionToDiscCenterSquared < 1);
}