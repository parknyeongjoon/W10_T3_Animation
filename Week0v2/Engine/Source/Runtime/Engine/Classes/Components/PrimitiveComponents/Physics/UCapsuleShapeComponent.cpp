#include "UCapsuleShapeComponent.h"
#include "Math/JungleMath.h"

UCapsuleShapeComponent::UCapsuleShapeComponent()
    : UShapeComponent()
    , CapsuleHalfHeight(2.0f) // Default capsule half height
    , CapsuleRaidus(1.0f)     // Default capsule radius
{
}

UCapsuleShapeComponent::UCapsuleShapeComponent(const UCapsuleShapeComponent& Other)
    : UShapeComponent(Other)
    , CapsuleHalfHeight(Other.CapsuleHalfHeight)
    , CapsuleRaidus(Other.CapsuleRaidus)
{
}

UCapsuleShapeComponent::~UCapsuleShapeComponent()
{
}

void UCapsuleShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

    UpdateBroadAABB();
}

void UCapsuleShapeComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void UCapsuleShapeComponent::UpdateBroadAABB()
{
    //FVector Center = GetComponentLocation();
    //FVector Rotation = GetComponentRotation().ToVector();
    //FVector Scale = GetComponentScale();

    //FMatrix WorldMatrix = JungleMath::CreateModelMatrix(Center, Rotation, Scale);

    float R = GetRadius();
    float H = GetHalfHeight();

    FMatrix WorldMatrix = GetWorldMatrix();

    FVector LocalCorners[8] = {
        {  R,  R,  H }, {  R,  R, -H },
        {  R, -R,  H }, {  R, -R, -H },
        { -R,  R,  H }, { -R,  R, -H },
        { -R, -R,  H }, { -R, -R, -H }
    };

    FVector W0 = WorldMatrix.TransformPosition(LocalCorners[0]); 
    FVector Min = W0;
    FVector Max = W0;

    for (int i = 1; i < 8; ++i)
    {
        FVector W = WorldMatrix.TransformPosition(LocalCorners[i]);
        Min.x = FMath::Min(Min.x, W.x);
        Min.y = FMath::Min(Min.y, W.y);
        Min.z = FMath::Min(Min.z, W.z);
            
        Max.x = FMath::Max(Max.x, W.x);
        Max.y = FMath::Max(Max.y, W.y);
        Max.z = FMath::Max(Max.z, W.z);
    }

    BroadAABB.min = Min;
    BroadAABB.max = Max;
}

bool UCapsuleShapeComponent::TestOverlaps(const UShapeComponent* OtherShape) const
{
    if (!BroadPhaseCollisionCheck(OtherShape))
    {
        return false;
    }

    return NarrowPhaseCollisionCheck(OtherShape);
}

bool UCapsuleShapeComponent::NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    return true;
}
