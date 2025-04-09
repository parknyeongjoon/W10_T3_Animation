#pragma once
#include "Engine/EngineTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class AActor;
struct FActorComponentInfo;

enum class EComponentOrigin
{
    None,
    Constructor,            // 생성자에서 추가한 컴포넌트
    Editor,                 // 에디터에서 추가한 컴포넌트
    Runtime,                // 런타임에서 추가한 컴포넌트
    Serialized,            // 직렬화된 컴포넌트
};

class UActorComponent : public UObject
{
    DECLARE_CLASS(UActorComponent, UObject)

private:
    friend class AActor;

public:
    UActorComponent() = default;
    UActorComponent(const UActorComponent& Other);

    /** AActor가 World에 Spawn되어 BeginPlay이전에 호출됩니다. */
    virtual void InitializeComponent();

    /** 컴포넌트가 제거되거나 소유자 Actor가 파괴될 때 호출됩니다. */
    virtual void UninitializeComponent();

    /** 모든 초기화가 끝나고, 준비가 되었을 때 호출됩니다. */
    virtual void BeginPlay();

    /** 매 틱마다 호출됩니다. */
    virtual void TickComponent(float DeltaTime);

    /** Component가 제거되었을 때 호출됩니다. */
    virtual void OnComponentDestroyed();

    /**
     * Ends gameplay for this component.
     * Called from AActor::EndPlay only
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
    /** 이 컴포넌트를 소유하고 있는 Actor를 반환합니다. */
    AActor* GetOwner() const { return Owner; }

    /** 이 컴포넌트를 제거합니다. */
    virtual void DestroyComponent();

    /** Component의 BeginPlay가 호출 되었는지 여부를 반환합니다. */
    bool HasBegunPlay() const { return bHasBegunPlay; }

    /** Component가 초기화 되었는지 여부를 반환합니다. */
    bool HasBeenInitialized() const { return bHasBeenInitialized; }

    /** Component가 현재 활성화 중인지 여부를 반환합니다. */
    bool IsActive() const { return bIsActive; }

    void Activate();
    void Deactivate();

    bool IsComponentTickEnabled() const { return bTickEnabled; }
    void SetComponentTickEnabled(bool bEnabled) { bTickEnabled = bEnabled; }

public:
    /** Tick을 아예 지원하는 컴포넌트인지 확인합니다. */
    bool CanEverTick() const { return bCanEverTick; }

    void RegisterComponent();
    void UnregisterComponent();

    bool IsRegistered() const { return bRegistered; }
    
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

protected:
    /**월드에 등록되었을 때 호출되는 함수*/
    virtual void OnRegister();
    virtual void OnUnregister();

public:
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo();
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);

protected:
    /** Tick을 지원하는 컴포넌트인지 여부 */
    uint8 bCanEverTick : 1;

    /** 컴포넌트가 Actor에 정상적으로 등록되었는지 여부 */
    uint8 bRegistered : 1;

    /** 생성 직후 InitializeComponent()를 자동 호출할지 여부 */
    uint8 bWantsInitializeComponent : 1;

private:
    AActor* Owner;

    /** InitializeComponent가 호출 되었는지 여부 */
    uint8 bHasBeenInitialized : 1;

    /** BeginPlay가 호출 되었는지 여부 */
    uint8 bHasBegunPlay : 1;

    /** 현재 컴포넌트가 삭제 처리중인지 여부 */
    uint8 bIsBeingDestroyed : 1;

    /** Component가 현재 활성화 중인지 여부 */
    uint8 bIsActive : 1;
    /** Tick 함수를 실행할지 여부*/
    bool bTickEnabled = true;

public:
    /** Component가 초기화 되었을 때, 자동으로 활성화할지 여부 */
    uint8 bAutoActive : 1;

public:
    EComponentOrigin ComponentOrigin = EComponentOrigin::None;
};
