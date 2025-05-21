#include "Actor.h"

#include "EditorEngine.h"
#include "Engine/World.h"
#include "Script/LuaManager.h"
#include "UserInterface/Console.h"

extern UEngine* GEngine;

void AActor::BeginPlay()
{

    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Component : CopyComponents)
    {
        Component->BeginPlay();
    }
}

void AActor::Tick(const float DeltaTime)
{

    if (!RootComponent)
    {
        return;
    }
    // TODO: 임시로 Actor에서 Tick 돌리기
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Component : CopyComponents)
    {
        if (Component && Component->IsComponentTickEnabled())
        {
            Component->TickComponent(DeltaTime);
        }
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
        GUObjectArray.MarkRemoveObject(this);
    }

    return IsActorBeingDestroyed();
}

void AActor::RemoveOwnedComponent(UActorComponent* Component)
{
    OwnedComponents.Remove(Component);
}

void AActor::InitializeComponents() const
{
    TArray<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComponent : Components)
    {
        // 먼저 컴포넌트를 등록 처리
        if (!ActorComponent->IsRegistered())
        {
            ActorComponent->RegisterComponent();
        }

        if (ActorComponent->bAutoActive && !ActorComponent->IsActive())
        {
            ActorComponent->Activate();
        }

        if (!ActorComponent->HasBeenInitialized())
        {
            ActorComponent->InitializeComponent();
        }
    }
}

void AActor::UninitializeComponents() const
{
    TArray<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComponent : Components)
    {
        if (ActorComponent->HasBeenInitialized())
        {
            ActorComponent->UninitializeComponent();
        }
        if (ActorComponent->IsRegistered())
        {
            ActorComponent->UnregisterComponent();
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

            if (OldRootComponent)
            {
                OldRootComponent->SetupAttachment(RootComponent);
            }
        }
        return true;
    }
    return false;
}

FVector AActor::GetActorLocation() const
{
    return RootComponent ? RootComponent->GetWorldLocation() : FVector::ZeroVector;
}

FRotator AActor::GetActorRotation() const
{
    return RootComponent ? RootComponent->GetWorldRotation() : FRotator();
}

FVector AActor::GetActorScale() const
{
    return RootComponent ? RootComponent->GetWorldScale() : FVector::ZeroVector;
}

FVector AActor::GetActorForwardVector() const
{
    return RootComponent ? RootComponent->GetWorldForwardVector() : FVector::ForwardVector;
}

FVector AActor::GetActorRightVector() const
{
    return RootComponent ? RootComponent->GetWorldRightVector() : FVector::RightVector;
}

FVector AActor::GetActorUpVector() const
{
    return RootComponent ? RootComponent->GetWorldUpVector() : FVector::UpVector;
}

bool AActor::SetActorLocation(const FVector& NewLocation) const
{
    if (RootComponent)
    {
        RootComponent->SetRelativeLocation(NewLocation);
        return true;
    }
    return false;
}

bool AActor::SetActorRotation(const FRotator& NewRotation) const
{
    if (RootComponent)
    {
        RootComponent->SetRelativeRotation(NewRotation);
        return true;
    }
    return false;
}

bool AActor::SetActorScale(const FVector& NewScale) const
{
    if (RootComponent)
    {
        RootComponent->SetRelativeScale(NewScale);
        return true;
    }
    return false;
}
UActorComponent* AActor::AddComponentByClass(UClass* ComponentClass, EComponentOrigin Origin)
{
    if (ComponentClass == nullptr)
    {
        return nullptr;
    }

    UActorComponent* NewComponent = ComponentClass->CreateObject<UActorComponent>(this);
    if (NewComponent == nullptr)
    {
        return nullptr;
    }

    OwnedComponents.Add(NewComponent);
    NewComponent->Owner = this;

    if (USceneComponent* NewSceneComponent = Cast<USceneComponent>(NewComponent))
    {
        if (RootComponent == nullptr)
        {
            RootComponent = NewSceneComponent;
        }
        else
        {
            NewSceneComponent->SetupAttachment(RootComponent);
        }
    }

    NewComponent->ComponentOrigin = Origin;
    NewComponent->InitializeComponent();

    return NewComponent;
}

UActorComponent* AActor::AddComponentByName(const FString& ComponentName, const EComponentOrigin Origin)
{
    UClass* ComponentClass = UClassRegistry::Get().FindClassByName(ComponentName);
    return AddComponentByClass(ComponentClass, Origin);
}

UActorComponent* AActor::AddComponent(UClass* InClass, FName InName, bool bTryRootComponent)
{
    if (!InClass)
    {
        UE_LOG(LogLevel::Error, TEXT("UActorComponent failed: ComponentClass is null."));
        return nullptr;
    }
    
    if (InClass->IsChildOf<UActorComponent>())
    {
        UActorComponent* Component = static_cast<UActorComponent*>(FObjectFactory::ConstructObject(InClass, this, InName));

        if (!Component)
        {
            UE_LOG(LogLevel::Error, TEXT("UActorComponent failed: Class '%s' is not derived from AActor."), *InClass->GetName());
            return nullptr;
        }
        
        OwnedComponents.Add(Component);
        Component->Owner = this;

        // 만약 SceneComponent를 상속 받았다면

        if (bTryRootComponent)
        {
            if (USceneComponent* SceneComp = Cast<USceneComponent>(Component))
            {
                if (RootComponent == nullptr)
                {
                    RootComponent = SceneComp;
                }
                // TODO: 나중에 RegisterComponent() 생기면 주석 해제
                // else
                // {
                //     SceneComp->SetupAttachment(RootComponent);
                // }
            }
        }

        /* ActorComponent가 Actor와 World에 등록이 되었다는 전제하에 호출됩니다 */
        if (!Component->HasBeenInitialized())
        {
            // TODO: RegisterComponent() 생기면 제거
            Component->InitializeComponent();
        }

        return Component;
    }
    
    UE_LOG(LogLevel::Error, TEXT("UActorComponent failed: ComponentClass is null."));
    return nullptr;
}

// AActor.cpp
void AActor::AddComponent(UActorComponent* Component)
{
    OwnedComponents.Add(Component);
    Component->Owner = this;

    if (USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
    {
        if (RootComponent == nullptr)
        {
            RootComponent = SceneComponent;
        }
        else
        {
            SceneComponent->SetupAttachment(RootComponent);
        }
    }

    Component->InitializeComponent();
}


UObject* AActor::Duplicate(UObject* InOuter)
{
    AActor* ClonedActor = Cast<ThisClass>(Super::Duplicate(InOuter));
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();

    return ClonedActor;
}

void AActor::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UObject::DuplicateSubObjects(Source, InOuter);

    const AActor* Actor = Cast<AActor>(Source);
    if (!Actor)
    {
        return;
    }

    TMap<const USceneComponent*, USceneComponent*> SceneCloneMap;
    
    for (UActorComponent* Component : Actor->OwnedComponents)
    {
        auto DuplicatedComponent = static_cast<UActorComponent*>(Component->Duplicate(this));
        DuplicatedComponent->Owner = this;
        OwnedComponents.Add(DuplicatedComponent);
        GetWorld()->GetLevel()->GetDuplicatedObjects().Add(Component, DuplicatedComponent);

        /** Todo. UActorComponent를 상속 받는 컴포넌트는 오류가 발생 코드 로직 수정 필요
         *   임시로 IsA 검사 후 Root 설정
         */
        if (DuplicatedComponent->IsA(USceneComponent::StaticClass())) 
        {
            RootComponent = Cast<USceneComponent>(DuplicatedComponent);
        }
        if (const USceneComponent* OldScene = Cast<USceneComponent>(Component))
        {
            if (USceneComponent* NewScene = Cast<USceneComponent>(DuplicatedComponent))
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

    if (Actor->RootComponent)
    {
        if (USceneComponent** Found = SceneCloneMap.Find(Actor->RootComponent))
        {
            SetRootComponent(*Found);
        }
    }

    // 컴포넌트 initialize
    for (const auto Component : OwnedComponents)
    {
        Component->InitializeComponent();
    }
}

void AActor::PostDuplicate()
{
    // Override in subclasses if needed
}

void AActor::LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray)
{
    // 생성자를 통해 만들어진 컴포넌트들은 이미 생성이 되어있는 상태
    // ActorComponentInfo의 ComponentOrigin을 보고 Constructor가 아니면 추가적으로 생성

    for (const auto& Info : InfoArray)
    {
        // 생성자에서 자동으로 생성되는 컴포넌트는 무시한다
        if (Info->Origin == EComponentOrigin::Constructor) continue;

        if (UClass* ComponentClass = UClassRegistry::Get().FindClassByName(Info->ComponentClass))
        {
            UActorComponent* NewComponent = AddComponentByClass(ComponentClass, EComponentOrigin::Serialized);
            if (Info->bIsRoot)
            {
                RootComponent = Cast<USceneComponent>(NewComponent);
            }
        }
    }
    if (InfoArray.Num() != OwnedComponents.Num())
    {
        UE_LOG(LogLevel::Error, "InfoArray.Num() != Components.Num()");
        return;
    }
    for (int i = 0; i < InfoArray.Num(); i++)
    {
        OwnedComponents[i]->LoadAndConstruct(*InfoArray[i]);
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
            // Component->GetComponentInfo()가 반환하는 unique_ptr의 소유권을
            // TArray 내부로 이동(move)시킵니다.
            ActorInfo.ComponentInfos.Add(std::move(Component->GetComponentInfo()));
        }
    }
    return ActorInfo;
}
