#include "MovementComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UMovementComponent::UMovementComponent()
{
    bUpdateOnlyIfRendered = false;
    bAutoUpdateTickRegistration = true;
    bAutoRegisterUpdatedComponent = true;
    
    bWantsInitializeComponent = true;
    bAutoActive = true;
    bInInitializeComponent = false;
}

UMovementComponent::UMovementComponent(const UMovementComponent& Other)
    : UActorComponent(Other)
{
    
}

float UMovementComponent::GetGravityZ() const
{
    return -0.981f; // Todo: Get gravity after physics volume work. 
}

void UMovementComponent::UpdateComponentVelocity()
{
    if (UpdatedComponent)
    {
        UpdatedComponent->ComponentVelocity = Velocity;
    }
}

void UMovementComponent::InitializeComponent()
{
    bInInitializeComponent = true;
    UActorComponent::InitializeComponent();

    UPrimitiveComponent* NewUpdatedComponent = nullptr;
    if (UpdatedComponent != nullptr)
    {
        NewUpdatedComponent = UpdatedComponent;
    }
    else if (bAutoRegisterUpdatedComponent)
    {
        if (AActor* MyActor = GetOwner())
        {
            NewUpdatedComponent = Cast<UPrimitiveComponent>(MyActor->GetRootComponent());
            if (!NewUpdatedComponent)
            {
                UE_LOG(LogLevel::Error, "NoRootPrimitiveWarning");
            }
        }
    }
    
    UpdatedComponent = NewUpdatedComponent;
    bInInitializeComponent = false;
}

void UMovementComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
    // TODO: Tick에서 매번 Owner 확인 안해도 되게 바꾸기...
    if (UpdatedComponent == nullptr)
        UpdatedComponent = dynamic_cast<UPrimitiveComponent*>(GetOwner()->GetRootComponent());
}

bool UMovementComponent::MoveComponent(const FVector& Delta)
{
    if (UpdatedComponent)
    {
        return UpdatedComponent->MoveComponent(Delta);
    }
    return false;
}

UObject* UMovementComponent::Duplicate() const
{
    UMovementComponent* NewComp = FObjectFactory::ConstructObjectFrom<UMovementComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void UMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    UActorComponent::DuplicateSubObjects(Source);

    UMovementComponent* SourceComp = Cast<UMovementComponent>(Source);
    if (SourceComp)
    {
        Velocity = SourceComp->Velocity;
        bUpdateOnlyIfRendered = SourceComp->bUpdateOnlyIfRendered;
        bAutoUpdateTickRegistration = SourceComp->bAutoUpdateTickRegistration;
        bAutoRegisterUpdatedComponent = SourceComp->bAutoRegisterUpdatedComponent;
        InitializeComponent();  // Set UpdatedComponent
    }
}

void UMovementComponent::PostDuplicate()
{
    UActorComponent::PostDuplicate();
}
