#include "ActorComponent.h"

#include "GameFramework/Actor.h"
#include "UObject/UObjectArray.h"

UActorComponent::UActorComponent()
    : ComponentID(FGuid::NewGuid())
{
}


void UActorComponent::InitializeComponent()
{
    assert(!bHasBeenInitialized);

    bHasBeenInitialized = true;
}

void UActorComponent::UninitializeComponent()
{
    assert(bHasBeenInitialized);

    bHasBeenInitialized = false;
}

void UActorComponent::BeginPlay()
{
    bHasBegunPlay = true;
}

void UActorComponent::TickComponent(float DeltaTime)
{
}

void UActorComponent::OnComponentDestroyed()
{
}

void UActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    assert(bHasBegunPlay);

    bHasBegunPlay = false;
}

AActor* UActorComponent::GetOwner() const
{
    return Owner;
}

void UActorComponent::DestroyComponent()
{
    if (bIsBeingDestroyed)
    {
        return;
    }

    bIsBeingDestroyed = true;

    // Owner에서 Component 제거하기
    if (AActor* MyOwner = GetOwner())
    {
        MyOwner->RemoveOwnedComponent(this);
        if (MyOwner->GetRootComponent() == this)
        {
            MyOwner->SetRootComponent(nullptr);
        }
    }

    if (bHasBegunPlay)
    {
        EndPlay(EEndPlayReason::Destroyed);
    }

    if (bHasBeenInitialized)
    {
        UninitializeComponent();
    }

    OnComponentDestroyed();

    // 나중에 ProcessPendingDestroyObjects에서 실제로 제거됨
    GUObjectArray.MarkRemoveObject(this);
}

void UActorComponent::OnRegister()
{
    // Hook: Called by RegisterComponent()
    if (bAutoActive)
    {
        Activate();
    }

    if (bWantsInitializeComponent && !bHasBeenInitialized)
    {
        InitializeComponent();
    }
}

void UActorComponent::OnUnregister()
{
    // Hook: Called by UnregisterComponent()
    Deactivate();
}

std::unique_ptr<FActorComponentInfo> UActorComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FActorComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void UActorComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    OutInfo.Origin = ComponentOrigin;
    OutInfo.ComponentID = ComponentID;
    OutInfo.ComponentClass = GetClass()->GetName();
    OutInfo.ComponentName = GetName();
    OutInfo.ComponentOwner = GetOwner() ? GetOwner()->GetName() : (TEXT("None"));
    OutInfo.bIsActive = bIsActive;
    OutInfo.bAutoActive = bAutoActive;
    OutInfo.bTickEnabled = bTickEnabled;
    OutInfo.bIsRoot = GetOwner() && (GetOwner()->GetRootComponent() == this);
}

void UActorComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    ComponentOrigin = Info.Origin;
    ComponentID = Info.ComponentID;
    SetFName(Info.ComponentName);
    bIsActive = Info.bIsActive;
    bAutoActive = Info.bAutoActive;
    bTickEnabled = Info.bTickEnabled;
}

void UActorComponent::RegisterComponent()
{
    if (bRegistered)
        return;

    bRegistered = true;
    OnRegister();
}

void UActorComponent::UnregisterComponent()
{
    if (!bRegistered)
        return;

    OnUnregister();
    bRegistered = false;
}
UObject* UActorComponent::Duplicate(UObject* InOuter)
{
    UActorComponent* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}
void UActorComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UObject::DuplicateSubObjects(Source, InOuter);
}
void UActorComponent::PostDuplicate()
{
    
}