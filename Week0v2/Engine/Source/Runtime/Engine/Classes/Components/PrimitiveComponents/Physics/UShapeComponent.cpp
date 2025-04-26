#include "UShapeComponent.h"
#include "Launch/EditorEngine.h"
#include "Components/SceneComponent.h"

UShapeComponent::UShapeComponent()
    : ShapeColor(FLinearColor::Green)
    , bDrawOnlyIfSelected(true)
    , PrevLocation(FVector::ZeroVector)
    , PrevRotation(FRotator(0, 0, 0))
    , PrevScale(FVector::OneVector)
{
}

UShapeComponent::UShapeComponent(const UShapeComponent& Other)
    : UPrimitiveComponent(Other)
    , ShapeColor(Other.ShapeColor)
    , bDrawOnlyIfSelected(Other.bDrawOnlyIfSelected)
    , PrevLocation(Other.GetPrevLocation())
    , PrevRotation(Other.GetPrevRotation())
    , PrevScale(Other.GetPrevScale)
{
}

UShapeComponent::~UShapeComponent()
{
}

void UShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

    bDrawOnlyIfSelected = true;

    UEditorEngine::CollisionManager.Register(this);
}

void UShapeComponent::TickComponent(float DeltaTime)
{
}

UObject* UShapeComponent::Duplicate() const
{
    UShapeComponent* NewComp = FObjectFactory::ConstructObjectFrom<UShapeComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();

    return NewComp;
}

void UShapeComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void UShapeComponent::PostDuplicate()
{
    Super::PostDuplicate();
}

bool UShapeComponent::TestOverlaps(const UShapeComponent* OtherShape) const
{
    return false;
}

bool UShapeComponent::BroadPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    return BroadAABB.IntersectAABB(OtherShape->GetBroadAABB());
}

bool UShapeComponent::NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    return false;
}
