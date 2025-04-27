#include "MovementComponent.h"

#include "Components/PrimitiveComponents/PrimitiveComponent.h"
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
    , Velocity(Other.Velocity)
    , bUpdateOnlyIfRendered(Other.bUpdateOnlyIfRendered)
    , bAutoUpdateTickRegistration(Other.bAutoUpdateTickRegistration)
    , bAutoRegisterUpdatedComponent(Other.bAutoRegisterUpdatedComponent)
    , bInInitializeComponent(Other.bInInitializeComponent)
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
}

void UMovementComponent::PostDuplicate()
{
    UActorComponent::PostDuplicate();
}

std::unique_ptr<FActorComponentInfo> UMovementComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FMovementComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void UMovementComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FMovementComponentInfo* Info = static_cast<FMovementComponentInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);

    Info->bUpdateOnlyIfRendered = bUpdateOnlyIfRendered;
    Info->bAutoUpdateTickRegistration = bAutoUpdateTickRegistration;
    Info->bAutoRegisterUpdatedComponent = bAutoRegisterUpdatedComponent;
    Info->Velocity = Velocity;

}

void UMovementComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FMovementComponentInfo& MovementInfo = static_cast<const FMovementComponentInfo&>(Info);
    bUpdateOnlyIfRendered = MovementInfo.bUpdateOnlyIfRendered;
    bAutoUpdateTickRegistration = MovementInfo.bAutoUpdateTickRegistration;
    bAutoRegisterUpdatedComponent = MovementInfo.bAutoRegisterUpdatedComponent;
    Velocity = MovementInfo.Velocity;
}
