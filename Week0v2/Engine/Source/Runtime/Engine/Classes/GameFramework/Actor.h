#pragma once
#include "Components/SceneComponent.h"
#include "Container/Set.h"
#include "Engine/EngineTypes.h"
#include "UObject/Casts.h"
#include "UObject/Object.h"
#include "UObject/ObjectFactory.h"
#include "UObject/ObjectMacros.h"
#include "ActorInfo.h"


class UActorComponent;

class AActor : public UObject
{
    DECLARE_CLASS(AActor, UObject)

public:
    AActor() = default;
    AActor(const AActor& Other);
    /** Actor가 게임에 배치되거나 스폰될 때 호출됩니다. */
    virtual void BeginPlay();

    /** 매 Tick마다 호출됩니다. */
    virtual void Tick(float DeltaTime);

    /** Actor가 제거될 때 호출됩니다. */
    virtual void Destroyed();

    /**
     * 액터가 게임 플레이를 종료할 때 호출되는 함수입니다.
     *
     * @param EndPlayReason EndPlay가 호출된 이유를 나타내는 열거형 값
     * @note Destroyed와는 다른점은, EndPlay는 레벨 전환, 게임 종료, 또는 Destroy() 호출 시 항상 실행됩니다.
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
    /** 이 Actor를 제거합니다. */
    virtual bool Destroy();

    /** 현재 Actor가 제거중인지 여부를 반환합니다. */
    bool IsActorBeingDestroyed() const
    {
        return bActorIsBeingDestroyed;
    }

    /**
     * Actor에 컴포넌트를 새로 추가합니다.
     * @tparam T UActorComponent를 상속받은 Component
     * @return 생성된 Component
     */
    template <typename T>
        requires std::derived_from<T, UActorComponent>
    T* AddComponent(EComponentOrigin Origin = EComponentOrigin::Constructor);

    // 클래스 정보를 바탕으로 컴포넌트를 새로 추가.
    UActorComponent* AddComponentByClass(UClass* ComponentClass, EComponentOrigin Origin);
    void AddComponent(UActorComponent* Component);
    /** Actor가 가지고 있는 Component를 제거합니다. */
    void RemoveOwnedComponent(UActorComponent* Component);

    /** Actor가 가지고 있는 모든 컴포넌트를 가져옵니다. */
    const TArray<UActorComponent*>& GetComponents() const { return OwnedComponents; }

    template<typename T>
        requires std::derived_from<T, UActorComponent>
    T* GetComponentByClass();

    void InitializeComponents();
    void UninitializeComponents();

public:
    USceneComponent* GetRootComponent() const { return RootComponent; }
    bool SetRootComponent(USceneComponent* NewRootComponent);

    AActor* GetOwner() const { return Owner; }
    void SetOwner(AActor* NewOwner) { Owner = NewOwner; }

public:
    FVector GetActorLocation() const { return RootComponent ? RootComponent->GetWorldLocation() : FVector::ZeroVector; }
    FVector GetActorRotation() const { return RootComponent ? RootComponent->GetWorldRotation() : FVector::ZeroVector; }
    FVector GetActorScale() const { return RootComponent ? RootComponent->GetWorldScale() : FVector::ZeroVector; }

    FVector GetActorForwardVector() const { return RootComponent ? RootComponent->GetForwardVector() : FVector::ForwardVector; }
    FVector GetActorRightVector() const { return RootComponent ? RootComponent->GetRightVector() : FVector::RightVector; }
    FVector GetActorUpVector() const { return RootComponent ? RootComponent->GetUpVector() : FVector::UpVector; }

    bool SetActorLocation(const FVector& NewLocation);
    bool SetActorRotation(const FVector& NewRotation);
    bool SetActorScale(const FVector& NewScale);
    
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

public:
    virtual void LoadAndConstruct(const TArray<std::shared_ptr<FActorComponentInfo>>& InfoArray);
    virtual FActorInfo GetActorInfo();

public:
    bool ShouldTickInEditor() const { return bTickInEditor; }
    void SetTickInEditor(bool bEnable) { bTickInEditor = bEnable; }
protected:
    USceneComponent* RootComponent = nullptr;

private:
    /** 이 Actor를 소유하고 있는 다른 Actor의 정보 */
    AActor* Owner = nullptr;

    /** 에디터 모드에서도 Tick이 작동하게 할 것인지 여부 */
    bool bTickInEditor = false;
    
    /** 본인이 소유하고 있는 컴포넌트들의 정보 */
    TArray<UActorComponent*> OwnedComponents;


    /** 현재 Actor가 삭제 처리중인지 여부 */
    uint8 bActorIsBeingDestroyed : 1;

#if 1 // TODO: WITH_EDITOR 추가
public:
    /** Actor의 기본 Label을 가져옵니다. */
    FString GetDefaultActorLabel() const;

    /** Actor의 Label을 가져옵니다. */
    FString GetActorLabel() const;

    /** Actor의 Label을 설정합니다. */
    void SetActorLabel(const FString& NewActorLabel);

private:
    /** 에디터상에 보이는 Actor의 이름 */
    FString ActorLabel;
#endif
};


template <typename T> requires std::derived_from<T, UActorComponent>
T* AActor::AddComponent(EComponentOrigin Origin)
{
    T* Component = FObjectFactory::ConstructObject<T>();
    OwnedComponents.Add(Component);
    Component->Owner = this;

    // 만약 SceneComponent를 상속 받았다면
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

    // TODO: RegisterComponent() 생기면 제거
    Component->InitializeComponent();
    Component->ComponentOrigin = Origin;

    return Component;
}


template <typename T> requires std::derived_from<T, UActorComponent>
T* AActor::GetComponentByClass()
{
    for (UActorComponent* Component : OwnedComponents)
    {
        if (T* CastedComponent = Cast<T>(Component))
        {
            return CastedComponent;
        }
    }
    return nullptr;
}