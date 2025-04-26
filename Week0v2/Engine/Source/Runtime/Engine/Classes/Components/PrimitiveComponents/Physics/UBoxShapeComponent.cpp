#include "UBoxShapeComponent.h"
#include "Math/JungleMath.h"

UBoxShapeComponent::UBoxShapeComponent()

{
}

UBoxShapeComponent::UBoxShapeComponent(const UBoxShapeComponent& Other)

{
}

UBoxShapeComponent::~UBoxShapeComponent()
{
}

void UBoxShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

    BoxExtent = FVector::OneVector;
    BroadAABB = FBoundingBox(FVector(-1, -1, -1), FVector(1, 1, 1));
    UpdateBroadAABB();
}

void UBoxShapeComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    // 현재 Transform
    const FVector CurLocation = GetComponentLocation();
    const FRotator CurRotation = GetComponentRotation();
    const FVector CurScale = GetComponentScale();

    if (PrevLocation != CurLocation || PrevRotation != CurRotation || PrevScale != CurScale)
    {
        UpdateBroadAABB();
     
        PrevLocation = CurLocation;
        PrevRotation = CurRotation;
        PrevScale = CurScale;
    }
}

void UBoxShapeComponent::UpdateBroadAABB()
{
    FVector Center = GetComponentLocation();
    FVector Scale = GetComponentScale();
    FVector Extent = BoxExtent * Scale /** 0.5f*/;

    FVector LocalCorners[8] =
    {
        FVector(-Extent.x, -Extent.y, -Extent.z),
        FVector(Extent.x, -Extent.y, -Extent.z),
        FVector(Extent.x,  Extent.y, -Extent.z),
        FVector(-Extent.x,  Extent.y, -Extent.z),
        FVector(-Extent.x, -Extent.y,  Extent.z),
        FVector(Extent.x, -Extent.y,  Extent.z),
        FVector(Extent.x,  Extent.y,  Extent.z),
        FVector(-Extent.x,  Extent.y,  Extent.z)
    };

    FMatrix RotationMatrix = GetRotationMatrix();

    FVector Min = FVector::ZeroVector;
    FVector Max = FVector::ZeroVector;
    
    for (int i = 0; i < 8; ++i)
    {
        FVector WorldCorner = FMatrix::TransformVector(LocalCorners[i], RotationMatrix);

        Min.x = FMath::Min(Min.x, WorldCorner.x);
        Min.y = FMath::Min(Min.y, WorldCorner.y);
        Min.z = FMath::Min(Min.z, WorldCorner.z);

        Max.x = FMath::Max(Max.x, WorldCorner.x);
        Max.y = FMath::Max(Max.y, WorldCorner.y);
        Max.z = FMath::Max(Max.z, WorldCorner.z);
    }

    BroadAABB.min = Center + Min;
    BroadAABB.max = Center + Max;
}

bool UBoxShapeComponent::TestOverlaps(const UShapeComponent* OtherShape) const
{
    if (!BroadPhaseCollisionCheck(OtherShape))
    {
        return false;
    }

    return NarrowPhaseCollisionCheck(OtherShape);
}

bool UBoxShapeComponent::NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    // 임시로 AABB로 처리
    return true;
}
