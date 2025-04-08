#include "Actor.h"

#include "Engine/World.h"

AActor::AActor(const AActor& Other)
    : UObject(Other),
      RootComponent(nullptr),
      bTickInEditor(Other.bTickInEditor),
      bActorIsBeingDestroyed(Other.bActorIsBeingDestroyed),
      ActorLabel(Other.ActorLabel),
    OwnedComponents(Other.OwnedComponents)
{
    OwnedComponents.Empty();
}
void AActor::BeginPlay()
{
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        Comp->BeginPlay();
    }
}

void AActor::Tick(float DeltaTime)
{
    if (!RootComponent) return;
    // TODO: 임시로 Actor에서 Tick 돌리기
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        if (Comp && Comp->IsComponentTickEnabled())
            Comp->TickComponent(DeltaTime);
    }
    // SetActorLocation(GetActorLocation() + FVector(1.0f, 0.0f, 0.0f));
}

void AActor::Destroyed()
{
    // Actor가 제거되었을 때 호출하는 EndPlay
    EndPlay(EEndPlayReason::Destroyed);
}

void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 본인이 소유하고 있는 모든 컴포넌트의 EndPlay 호출
    for (UActorComponent* Component : GetComponents())
    {
        if (Component->HasBegunPlay())
        {
            Component->EndPlay(EndPlayReason);
        }
    }
    UninitializeComponents();
}


bool AActor::Destroy()
{
    if (!IsActorBeingDestroyed())
    {
        if (UWorld* World = GetWorld())
        {
            World->DestroyActor(this);
            bActorIsBeingDestroyed = true;
        }
    }

    return IsActorBeingDestroyed();
}

void AActor::RemoveOwnedComponent(UActorComponent* Component)
{
    OwnedComponents.Remove(Component);
}

void AActor::InitializeComponents()
{
    TArray<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        // 먼저 컴포넌트를 등록 처리
        if (!ActorComp->IsRegistered())
        {
            ActorComp->RegisterComponent();
        }

        if (ActorComp->bAutoActive && !ActorComp->IsActive())
        {
            ActorComp->Activate();
        }

        if (!ActorComp->HasBeenInitialized())
        {
            ActorComp->InitializeComponent();
        }
    }
}

void AActor::UninitializeComponents()
{
    TArray<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        if (ActorComp->HasBeenInitialized())
        {
            ActorComp->UninitializeComponent();
        }
        if (ActorComp->IsRegistered())
        {
            ActorComp->UnregisterComponent();
        }
    }
}

bool AActor::SetRootComponent(USceneComponent* NewRootComponent)
{
    if (NewRootComponent == nullptr || NewRootComponent->GetOwner() == this)
    {
        if (RootComponent != NewRootComponent)
        {
            USceneComponent* OldRootComponent = RootComponent;
            RootComponent = NewRootComponent;

            OldRootComponent->SetupAttachment(RootComponent);
        }
        return true;
    }
    return false;
}

bool AActor::SetActorLocation(const FVector& NewLocation)
{
    if (RootComponent)
    {
        RootComponent->SetLocation(NewLocation);
        return true;
    }
    return false;
}

bool AActor::SetActorRotation(const FVector& NewRotation)
{
    if (RootComponent)
    {
        RootComponent->SetRotation(NewRotation);
        return true;
    }
    return false;
}

bool AActor::SetActorScale(const FVector& NewScale)
{
    if (RootComponent)
    {
        RootComponent->SetScale(NewScale);
        return true;
    }
    return false;
}
UActorComponent* AActor::AddComponentByClass(UClass* ComponentClass, EComponentOrigin Origin)
{
    if (ComponentClass == nullptr)
        return nullptr;

    UActorComponent* Component = ComponentClass->CreateObject<UActorComponent>();
    if (Component == nullptr)
        return nullptr;

    OwnedComponents.Add(Component);
    Component->Owner = this;

    if (USceneComponent* NewSceneComp = Cast<USceneComponent>(Component))
    {
        if (RootComponent == nullptr)
        {
            RootComponent = NewSceneComp;
        }
        else
        {
            NewSceneComp->SetupAttachment(RootComponent);
        }
    }

    Component->ComponentOrigin = Origin;
    Component->InitializeComponent();

    return Component;
}
// AActor.cpp
void AActor::AddComponent(UActorComponent* Component)
{
    OwnedComponents.Add(Component);
    Component->Owner = this;

    if (USceneComponent* SceneComp = Cast<USceneComponent>(Component))
    {
        if (RootComponent == nullptr)
        {
            RootComponent = SceneComp;
        }
        else
        {
            SceneComp->SetupAttachment(RootComponent);
        }
    }

    Component->InitializeComponent();
}


UObject* AActor::Duplicate() const
{
    AActor* ClonedActor = FObjectFactory::ConstructObjectFrom<AActor>(this);
    ClonedActor->DuplicateSubObjects(this);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}
void AActor::DuplicateSubObjects(const UObject* SourceObj)
{
    UObject::DuplicateSubObjects(SourceObj);

    const AActor* Source = Cast<AActor>(SourceObj);
    if (!Source) return;

    TMap<const USceneComponent*, USceneComponent*> SceneCloneMap;

    for (UActorComponent* Component : Source->OwnedComponents)
    {
        UActorComponent* dupComponent = static_cast<UActorComponent*>(Component->Duplicate());
        dupComponent->Owner = this;
        OwnedComponents.Add(dupComponent);
        RootComponent = Cast<USceneComponent>(dupComponent);
        if (const USceneComponent* OldScene = Cast<USceneComponent>(Component))
        {
            if (USceneComponent* NewScene = Cast<USceneComponent>(dupComponent))
            {
                SceneCloneMap.Add(OldScene, NewScene);
            }
        }
    }

    for (const auto& Pair : SceneCloneMap)
    {
        const USceneComponent* Old = Pair.Key;
        USceneComponent* New = Pair.Value;
        if (const USceneComponent* OldParent = Old->GetAttachParent())
        {
            if (USceneComponent** Found = SceneCloneMap.Find(OldParent))
            {
                USceneComponent* NewParent = *Found;
                New->SetupAttachment(NewParent);
            }
        }
    }

    if (Source->RootComponent)
    {
        if (USceneComponent** Found = SceneCloneMap.Find(Source->RootComponent))
        {
            SetRootComponent(*Found);
        }
    }
}

void AActor::PostDuplicate()
{
    // Override in subclasses if needed
}

void AActor::LoadAndConstruct(const TArray<FActorComponentInfo>& InfoArray)
{
    // 생성자를 통해 만들어진 컴포넌트들은 이미 생성이 되어있는 상태
    // ActorComponentInfo의 ComponentOrigin을 보고 Constructor가 아니면 추가적으로 생성

    for (const FActorComponentInfo& Info : InfoArray)
    {
        // 생성자에서 자동으로 생성되는 컴포넌트는 무시한다
        if (Info.Origin == EComponentOrigin::Constructor) continue;

        UClass* ComponentClass = UClassRegistry::Get().FindClassByName(Info.Type);
        if (ComponentClass)
        {
            UActorComponent* NewComp = AddComponentByClass(ComponentClass, EComponentOrigin::Serialized);
            if (Info.bIsRoot)
                RootComponent = Cast<USceneComponent>(NewComp);
        }
    }
    if (InfoArray.Num() != OwnedComponents.Num())
    {
        UE_LOG(LogLevel::Error, "InfoArray.Num() != Components.Num()");
        return;
    }

    for (int i = 0; i < InfoArray.Num(); i++)
    {
        OwnedComponents[i]->LoadAndConstruct(InfoArray[i]);
    }
}

FActorInfo AActor::GetActorInfo()
{
    FActorInfo ActorInfo;
    ActorInfo.Type = GetClass()->GetName();
    for (UActorComponent* Component : OwnedComponents)
    {
        if (Component)
        {
            FActorComponentInfo ComponentInfo;
            ComponentInfo.Type = Component->GetClass()->GetName();
            ActorInfo.ComponentInfos.Add(ComponentInfo);
        }
    }
    return FActorInfo();
}
