#include "UShapeComponent.h"
#include "Launch/EditorEngine.h"

UShapeComponent::UShapeComponent()
{
}

UShapeComponent::UShapeComponent(const UShapeComponent& Other)
{
}

UShapeComponent::~UShapeComponent()
{
}

void UShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

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
    return false;
}

bool UShapeComponent::NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    return false;
}
