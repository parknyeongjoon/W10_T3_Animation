#include "USphereShapeComponent.h"
#include "Math/JungleMath.h"

USphereShapeComponent::USphereShapeComponent()
    : UShapeComponent()
    , Radius(1.0f) // Default radius
{
}

USphereShapeComponent::USphereShapeComponent(const USphereShapeComponent& Other)  
   : UShapeComponent(Other) 
   , Radius(Other.Radius)    
{  
}

USphereShapeComponent::~USphereShapeComponent()
{
}

void USphereShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

    UpdateBroadAABB();
}

void USphereShapeComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void USphereShapeComponent::UpdateBroadAABB()
{
    FVector Center = GetComponentLocation();
    FVector Rotation = GetComponentRotation().ToVector();
    FVector Scale = GetComponentScale();

    float R = GetRadius();
    FVector ScaledRadius = Scale * R;

    FMatrix WorldMatrix = JungleMath::CreateModelMatrix(Center, Rotation, ScaledRadius);

    FVector LocalCorners[8] = {
        { ScaledRadius.x,  ScaledRadius.y,  ScaledRadius.z },
        { ScaledRadius.x,  ScaledRadius.y, -ScaledRadius.z },
        { ScaledRadius.x, -ScaledRadius.y,  ScaledRadius.z },
        { ScaledRadius.x, -ScaledRadius.y, -ScaledRadius.z },
        { -ScaledRadius.x,  ScaledRadius.y,  ScaledRadius.z },
        { -ScaledRadius.x,  ScaledRadius.y, -ScaledRadius.z },
        { -ScaledRadius.x, -ScaledRadius.y,  ScaledRadius.z },
        { -ScaledRadius.x, -ScaledRadius.y, -ScaledRadius.z }
    };

    FVector WorldPt0 = WorldMatrix.TransformPosition(LocalCorners[0]);
    FVector Min = WorldPt0;
    FVector Max = WorldPt0;

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
    // 임시로 AABB로 처리
    return true;
}
