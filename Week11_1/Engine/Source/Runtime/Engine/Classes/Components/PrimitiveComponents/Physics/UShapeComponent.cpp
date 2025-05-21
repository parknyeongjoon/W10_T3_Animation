#include "UShapeComponent.h"
#include "Launch/EditorEngine.h"
#include "Components/SceneComponent.h"
#include "Physics/FCollisionManager.h"
#include "CoreUObject/UObject/Casts.h"

UShapeComponent::UShapeComponent()
    : ShapeColor(FLinearColor::Green)
    , bDrawOnlyIfSelected(true)
    , PrevLocation(FVector::ZeroVector)
    , PrevRotation(FRotator(0, 0, 0))
    , PrevScale(FVector::OneVector)
{
}


UShapeComponent::~UShapeComponent()
{
    
}

void UShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

    bDrawOnlyIfSelected = true;
    BroadAABB = FBoundingBox(FVector(-1, -1, -1), FVector(1, 1, 1));
}

void UShapeComponent::BeginPlay()
{
    UEditorEngine::CollisionManager.Register(this);
}

void UShapeComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    const FVector CurLocation = GetWorldLocation();
    const FRotator CurRotation = GetWorldRotation();
    const FVector CurScale = GetWorldScale();

    if (PrevLocation != CurLocation || PrevRotation != CurRotation || PrevScale != CurScale)
    {
        UpdateBroadAABB();

        PrevLocation = CurLocation;
        PrevRotation = CurRotation;
        PrevScale = CurScale;
    }
}

void UShapeComponent::DestroyComponent()
{
    Super::DestroyComponent();
    UEditorEngine::CollisionManager.Unregister(this);
}

UObject* UShapeComponent::Duplicate(UObject* InOuter)
{
    UShapeComponent* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();

    return NewComp;
}

void UShapeComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
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
