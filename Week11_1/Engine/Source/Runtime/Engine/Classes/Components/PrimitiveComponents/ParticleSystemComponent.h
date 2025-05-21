#pragma once
#include "PrimitiveComponent.h"
#include "Container/Array.h"
#include "Container/EnumAsByte.h"

class UParticleSystem;
struct FParticleEmitterInstance;
struct FDynamicEmitterDataBase;
struct FDynamicEmitterReplayDataBase;
#pragma region structs

enum EParticleSysParamType : int
{
    PSPT_None,
    PSPT_Scalar,
    PSPT_ScalarRand,
    PSPT_Vector,
    PSPT_VectorRand,
    PSPT_Color,
    PSPT_Actor,
    PSPT_Material,
    PSPT_VectorUnitRand,
    PSPT_MAX,
};

enum EParticleEventType : int
{
    /** 모든 이벤트 허용 */
    EPET_Any,
    /** 파티클 생성 이벤트 */
    EPET_Spawn,
    /** 파티클 소멸 이벤트 */
    EPET_Death,
    /** 파티클 충돌 이벤트 */
    EPET_Collision,
    /** 파티클 버스트(대량 생성) 이벤트 */
    EPET_Burst,
    /** 블루프린트에서 생성된 이벤트 */
    EPET_Blueprint,
    EPET_MAX,
};

/**
 * 파티클 시스템 매개변수를 정의하는 구조체
 */
struct FParticleSysParam
{
    //GENERATED_USTRUCT_BODY()

    /** 매개변수 이름 */
    FName Name;

    /**
     * 매개변수 타입
     * PSPT_None           - 타입 없음
     * PSPT_Scalar         - 단일 실수 값 사용
     * PSPT_ScalarRand     - [Scalar_Low..Scalar] 범위 내 무작위 실수 선택
     * PSPT_Vector         - 벡터 값 사용
     * PSPT_VectorRand     - [Vector_Low..Vector] 범위 내 무작위 벡터 선택
     * PSPT_Color          - 색상 값 사용
     * PSPT_Actor          - 액터 참조 사용
     * PSPT_Material       - 머티리얼 참조 사용
     * PSPT_VectorUnitRand - 단위 벡터를 무작위 선택 후 [Vector_Low..Vector] 범위로 스케일
     */
    TEnumAsByte<enum EParticleSysParamType> ParamType;

    /** 실수 값 */
    float Scalar;

    /** 실수 값의 최솟값 (랜덤 범위용) */
    float Scalar_Low;

    /** 벡터 값 */
    FVector Vector;

    /** 벡터 값의 최솟값 (랜덤 범위용) */
    FVector Vector_Low;

    /** 색상 값 */
    FColor Color;

    /** 연관된 액터 */
    class AActor* Actor;

    /**
     * 사용할 머티리얼
     * TODO: 나중에 Texture 등 추가 가능
     */
    class UMaterialInterface* Material;

    FParticleSysParam()
        : ParamType(PSPT_None)
        , Scalar(0.0f)
        , Scalar_Low(0.0f)
        , Vector(FVector::ZeroVector)
        , Vector_Low(FVector::ZeroVector)
        , Color(FColor::Black)
        , Actor(nullptr)
        , Material(nullptr)
        , AsyncActorToWorld(FTransform::Identity)
        , AsyncActorVelocity(FVector::ZeroVector)
        , bAsyncDataCopyIsValid(false)
    {
    }

    /**
     * 게임 스레드에서 액터의 Transform, Velocity를 비동기 캐시
     */
    void UpdateAsyncActorCache()
    {
        if (Actor)
        {
            //AsyncActorToWorld = Actor->ActorToWorld();
            //AsyncActorVelocity = Actor->GetVelocity();
        }
        bAsyncDataCopyIsValid = true;
    }

    /** 비동기 캐시 초기화 */
    void ResetAsyncActorCache()
    {
        bAsyncDataCopyIsValid = false;
    }

    /**
     * 비동기 캐시된 Transform 반환
     * 캐시 유효 시 사용, 아니면 즉시 Actor에서 조회
     */
    FTransform GetAsyncActorToWorld() const
    {
        if (bAsyncDataCopyIsValid)
        {
            return AsyncActorToWorld;
        }
        else if (Actor)
        {
            //return Actor->ActorToWorld();
        }
        return FTransform::Identity;
    }

    /**
     * 비동기 캐시된 Velocity 반환
     * 캐시 유효 시 사용, 아니면 즉시 Actor에서 조회
     */
    FVector GetAsyncActorVelocity() const
    {
        if (bAsyncDataCopyIsValid)
        {
            return AsyncActorVelocity;
        }
        else if (Actor)
        {
            //return Actor->GetVelocity();
        }
        return FVector::ZeroVector;
    }

private:
    /** 비동기 Transform 캐시 */
    FTransform AsyncActorToWorld;
    /** 비동기 Velocity 캐시 */
    FVector AsyncActorVelocity;
    /** 캐시 유효 여부 */
    bool bAsyncDataCopyIsValid;
};

template <> struct TIsPODType<FParticleSysParam> { enum { Value = true }; };

/**
 * 모든 파티클 이벤트 데이터의 기본 구조체
 */
struct FParticleEventData
{
    /** 이벤트 타입 */
    int32 Type;

    /** 이벤트 이름 */
    FName EventName;

    /** 이벤트 발생 시점의 방출 시간 */
    float EmitterTime;

    /** 이벤트 위치 */
    FVector Location;

    /** 이벤트 시점의 속도 */
    FVector Velocity;

    /** 게임 전용 메타데이터 */
    TArray<class UParticleModuleEventSendToGame*> EventData;

    FParticleEventData()
        : Type(0)
        , EmitterTime(0.0f)
    {
    }
};

/**
 * 이벤트 발생 시 이미 존재하던 파티클의 추가 정보
 */
struct FParticleExistingData : public FParticleEventData
{
    /** 이벤트 시점까지의 파티클 생존 시간 */
    float ParticleTime;

    /** 이벤트 시점의 방향 */
    FVector Direction;

    FParticleExistingData()
        : ParticleTime(0.0f)
        , Direction(FVector::ZeroVector)
    {
    }
};

/**
 * 파티클 생성 이벤트 데이터
 */
struct FParticleEventSpawnData : public FParticleEventData
{
};

/**
 * 파티클 소멸(죽음) 이벤트 데이터
 */
struct FParticleEventDeathData : public FParticleExistingData
{
};

/**
 * 파티클 충돌 이벤트 데이터
 */
struct FParticleEventCollideData : public FParticleExistingData
{
    /** 충돌 시 법선 벡터 */
    FVector Normal;

    /** 충돌까지 남은 시간 (라인 체크 시) */
    float Time;

    /** 충돌한 프리미티브 인덱스, INDEX_NONE이면 없음 */
    int32 Item;

    /** 충돌한 본(스켈레탈 메시) 이름 */
    FName BoneName;

    /** 물리 소재(Physical Material) */
    //UPhysicalMaterial* PhysMat;

    FParticleEventCollideData()
        : Normal(FVector::ZeroVector)
        , Time(0.0f)
        , Item(INDEX_NONE)
    {
    }
};

/**
 * 파티클 버스트 이벤트 데이터
 */
struct FParticleEventBurstData : public FParticleEventData
{
    /** 생성된 파티클 수 */
    int32 ParticleCount;

    FParticleEventBurstData()
        : ParticleCount(0)
    {
    }
};

/**
 * (블루프린트) 파티클 이벤트 데이터
 */
struct FParticleEventKismetData : public FParticleEventData
{
};
#pragma endregion

class UParticleSystemComponent : public UPrimitiveComponent
{
    friend class FParticleSystemWorldManager;
    
    DECLARE_CLASS(UParticleSystemComponent, UPrimitiveComponent)
public:
    UParticleSystemComponent();
    ~UParticleSystemComponent() override;
    
    //이게 인스턴스, 틱돌면서 이거 돌아야함. 이거 데이터 기반으로 EmitterRenderData 생성
    UPROPERTY(EditAnywhere, TArray<FParticleEmitterInstance*>, EmitterInstances, = {})

    //얘는 원본데이터
    UParticleSystem* Template;
    
    //이게 렌더할 때 필요한 데이터 렌더할때 이거 돌면서 렌더
    //이 데이터가 Emitter가 뿜어낸 모든 Particle
    TArray<FDynamicEmitterDataBase*> EmitterRenderData;

#pragma region hide
    
    //TArray<UMaterialInterface*> EmitterMaterials;

    bool bWasCompleted;
    
    bool bSuppressSpawning;

    bool bWasDeactivated;

    /** 이 시스템이 등록 해제(unregistered)되거나 재설정(reset)되기 전에 활성화된 상태였다면 true입니다. 만약 그렇다면 다시 활성화됩니다 */
    bool bWasActive;

    /** true이면, 이 컴포넌트의 리셋이 요청되었음을 나타냅니다 */
    bool bResetTriggered;

    /** true이면, 이 컴포넌트의 비활성화(deactivate)가 요청되었음을 나타냅니다 */
    bool bDeactivateTriggered;
    
    bool bResetOnDetach;

    /** 컴포넌트가 등록된 이후 아직 Tick이 한 번도 호출되지 않았음을 나타냅니다 */
    bool bJustRegistered;

    /**
     * 이 플래그는 PSysComp가 처음 활성화될 때 설정됩니다.
     * 레벨 로드 시 자동 활성화된 PSysComp가 InitParticles를 두 번 호출하지 않도록 방지하는 데 사용됩니다.
     */
    bool bHasBeenActivated;

    /**
     * true이면, 이 파티클 시스템이 완료된 후 재활용(recycling)될 수 있습니다.
     * 자동 삭제(auto-destroy)되는 시스템은 재활용되지 않습니다.
     * 일부 시스템(현재 파티클 트레일 등)은 새 이펙트를 위해 컴포넌트를 재호출(respawn)하지 않고 재활용합니다.
     * 이는 최적화를 위한 것이며, 정상적인 컴포넌트 초기화 이벤트가 한 번만 호출된다는 것 외에는 동작이 변경되지 않습니다.
     */
    bool bAllowRecycling;

    /**
     * true이면, 활성화 시 AutoAttachParent에 자동으로 부착되고 완료 시 부모에서 분리(detach)됩니다.
     * 이는 AutoAttachParent가 null일 때 초기 부착을 활성화 시점까지 연기하며,
     * 할당 여부와 관계없이 분리 후 활성화 시점의 상대 변환(transform)을 복원합니다.
     * 또한 전용 서버에서는 bAutoActivate가 true라도 실제로 활성화하지 않으므로 부착이 비활성화됩니다.
     * @see AutoAttachParent, AutoAttachSocketName, AutoAttachLocationType
     */
    bool bAutoManageAttachment;

    /**
     * bAutoManageAttachment가 true일 때 AutoAttachParent에 부착할 때
     * WeldSimulatedBodies 옵션을 처리하는 방식에 대한 설정입니다.
     * @see bAutoManageAttachment
     */
    bool bAutoAttachWeldSimulatedBodies;
    
    /** 이 컴포넌트가 게임 플레이 코드에 의해 시그니피컨스(significance)가 관리되고 있는지 여부 */
    bool bIsManagingSignificance;
    /**
     * 이 컴포넌트가 이전에 게임 플레이 코드에 의해 시그니피컨스(significance)가 관리되었는지 여부.
     * 이 값이 변경되면 렌더 데이터 갱신이 필요합니다.
     */
    bool bWasManagingSignificance;

    bool bIsDuringRegister;
    
    bool bWarmingUp;
    

    /**
     * Tick 중에 동적 렌더 데이터 업데이트(dynamic updating of render data)를 수행하지 않도록 표시하는 플래그입니다.
     * 이는 주로 워밍업(warm up)과 특정 상태의 시뮬레이션된 이펙트에 사용됩니다.
     */
    bool bSkipUpdateDynamicDataDuringTick;

    /** "Tick 금지" 타임아웃 값이 발동되면 이 플래그가 설정됩니다 */
    bool bForcedInActive;

    /** true이면, ViewRelevanceFlags가 변경되어 다시 수집해야 함을 나타냅니다 */
    bool bIsViewRelevanceDirty;

    bool bAutoDestroy;

private:
    /** 컴포넌트의 변환(transform)이 변경되어 업데이트가 필요한지 여부 */
    bool bIsTransformDirty;

    /** 활성화 시 자동 부착(auto attach)이 발생했는지 여부. 분리(detach) 시 상대 변환을 복원할지 결정하는 데 사용 */
    bool bDidAutoAttach;

    /** AsyncComponentToWorld 등의 비동기 데이터 복사가 유효한지 여부 */
    bool bAsyncDataCopyIsValid;

    //0 [ManagerHandle] 30[bPendingManagerAdd] 31[bPendingManagerRemove]
    /** FParticleSystemWorldManager에 대한 핸들입니다. 관리되는 틱이 없으면 INDEX_NONE을 가집니다. */
    int32 ManagerHandle : 30;
    /** 현재 매니저에 추가 대기 중임을 나타내는 플래그입니다. */
    int32 bPendingManagerAdd : 1;
    /** 언레지스터되어 매니저 배열에서 제거 대기 중임을 나타내는 플래그입니다. */
    int32 bPendingManagerRemove : 1;
    
public:
    virtual bool Editor_CanBeTickManaged()const { return true; }
    bool ShouldBeTickManaged()const;

    FORCEINLINE bool IsTickManaged()const { return ManagerHandle != INDEX_NONE && !IsPendingManagerRemove(); }
    FORCEINLINE int32 GetManagerHandle()const { return ManagerHandle; }
    FORCEINLINE void SetManagerHandle(const int32 InHandle) { ManagerHandle = InHandle; }

    FORCEINLINE int32 IsPendingManagerAdd()const { return bPendingManagerAdd; }
    FORCEINLINE void SetPendingManagerAdd(const bool bValue) { bPendingManagerAdd = bValue; }
    FORCEINLINE int32 IsPendingManagerRemove()const { return bPendingManagerRemove; }
    FORCEINLINE void SetPendingManagerRemove(const bool bValue) { bPendingManagerRemove = bValue; }
    
    FParticleSystemWorldManager* GetWorldManager() const;

    /**
    * 이 ParticleSystemComponent에 대한 이름 붙은 인스턴스 매개변수 배열입니다.
    * DistributionFloat/VectorParticleParameters를 사용하여 Cascade에서 매개변수를 사용할 수 있습니다.
    */
    TArray<struct FParticleSysParam> InstanceParameters;

    void ResetNextTick()
    {
        bResetTriggered = true;
    }

    void DeactivateNextTick()
    {
        bDeactivateTriggered = true;
    }
    
    bool bOldPositionValid;

    FVector OldPosition;

    FVector PartSysVelocity;

    float WarmupTime;

    float WarmupTickRate;

    /**
     * 이 이미터가 렌더링되지 않은 상태로 유지되어야 하는 시간(초)입니다.
     * 이 시간이 지나면 더 이상 틱되지 않거나 비활성화됩니다.
     */
    float SecondsBeforeInactive;

private:
    /** 마지막으로 강제 UpdateTransform이 호출된 이후 경과된 시간을 추적합니다. */
    float TimeSinceLastForceUpdateTransform;

public:
    /**
     * 동적으로 계산된 바운드를 사용하는 시스템에서
     * 강제 UpdateTransform 사이의 시간 간격(초)입니다.
     * 사실상 바운드가 축소되는 주기를 의미합니다.
     */
    float MaxTimeBeforeForceUpdateTransform;

    /**
     * 레벨 내 파티클 시스템에 Cascade의 디테일 모드 설정을 적용할 때 사용합니다.
     */
    int32 EditorDetailMode;

    /** 
    * 시스템이 보이지 않을 때 틱을 건너뛸 수 있는지 판단하기 위해 
    * 총 틱 시간을 누적하는 데 사용합니다. 
    */
    float AccumTickTime;

    /** 
     * 마지막으로 중요하다고 간주된 이후 경과된 시간(초)입니다. 
     */
    float LastSignificantTime;
    
    /** 
    * 이 객체는 시작 시 생성되며, 이후 각 이터미터에 추가됩니다 
    */
    float EmitterDelay;

    /** Scales DeltaTime in UParticleSystemComponent::Tick(...) */
    float CustomTimeDilation;

    /** 이 PSysComp에서 발생한 Spawn 이벤트들입니다. */
    TArray<struct FParticleEventSpawnData> SpawnEvents;

    /** 이 PSysComp에서 발생한 Death(소멸) 이벤트들입니다. */
    TArray<struct FParticleEventDeathData> DeathEvents;

    /** 이 PSysComp에서 발생한 Collision(충돌) 이벤트들입니다. */
    TArray<struct FParticleEventCollideData> CollisionEvents;

    /** 이 PSysComp에서 발생한 Burst(버스트) 이벤트들입니다. */
    TArray<struct FParticleEventBurstData> BurstEvents;

    /** 이 PSysComp에서 발생한 Kismet(블루프린트) 이벤트들입니다. */
    TArray<struct FParticleEventKismetData> KismetEvents;

    /**
    * bAutoManageAttachment이 true일 때 활성화 시 자동으로 부착할 컴포넌트입니다.
    * 등록 시점에 null이면 기존 AttachParent를 할당하고, 활성화될 때까지 부착을 지연합니다.
    * @see bAutoManageAttachment
    */
    USceneComponent* AutoAttachParent;
    
private:
    /**
     * 자동 부착 이전에 저장된 상대 변환입니다.
     * 자동으로 부착된 경우 분리 시 이 변환을 복원하는 데 사용됩니다.
     */
    FVector SavedAutoAttachRelativeLocation;
    FRotator SavedAutoAttachRelativeRotation;
    FVector SavedAutoAttachRelativeScale3D;
private:
    /** Instance의 Cached된 복사본 */
    TArray<struct FParticleSysParam> AsyncInstanceParameters;

    /** 바운드의 캐시된 복사본입니다. */
    FBoundingBox AsyncBounds;

    /** 파티클 시스템 속도의 캐시된 복사본입니다. */
    FVector AsyncPartSysVelocity;

public:
    virtual UParticleSystem* GetFXSystemAsset() const { return Template; }
    
private:
    /** Cached copy of the transform for async work */
    FTransform AsyncComponentToWorld;

    /** TickComponent에 전달된 델타타임의 복사본 */
    float DeltaTimeTick;
    /** 비동기 틱에서 얻은 정보 */
    int32 TotalActiveParticles;
    /** 중요하다고 간주되는 이터미터의 개수. 이 값이 0이면 시스템이 비활성화되거나 틱을 중지할 수 있습니다. */
    uint32 NumSignificantEmitters;
    /** 마지막 틱 이후 경과된 시간(밀리초). UParticleSystem의 MinTimeBetweenTicks와 함께 틱 간격을 제어하는 데 사용됩니다. */
    uint32 TimeSinceLastTick;

    void SetComponentTickEnabled(bool bEnabled) override;
    bool IsComponentTickEnabled() const override;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;

    void ComputeTickComponent_Concurrent(float DeltaTime);
    void FinalizeTickComponent();

protected:
    virtual bool ShouldActivate() const;
public:
    // 이걸 호출 해야 파티클 동작, 아마 Particle Emmiter Spawn 후, 바로 이거 호출하면 될 듯?
    virtual void Activate(bool bReset=false);
    // 이걸 호출 해야 파티클 정지, 아마 Particle Emmiter DeSpawn 할 때, 호출하면 될 듯?
    void Deactivate() override;
    
    void Complete();
    virtual void DeactivateImmediate();

    void ForceReset();

    /** Activate the system */
    virtual void ActivateSystem(bool bFlagAsJustAttached = false);
    /** Deactivate the system */

    void DeactivateSystem();

    //FParticleDynamicData* CreateDynamicData(ERHIFeatureLevel::Type InFeatureLevel);
public:

    // @todo document
    void ResetParticles(bool bEmptyInstances = false);
    
    // @todo document
    void ResetBurstLists();
    
    // @todo document
    bool HasCompleted();
    
    // @todo document
    void InitializeSystem();

    void ReportEventSpawn(const FName InEventName, const float InEmitterTime,
        const FVector InLocation, const FVector InVelocity, const TArray<class UParticleModuleEventSendToGame*>& InEventData);

    void ReportEventDeath(const FName InEventName, const float InEmitterTime,
        const FVector InLocation, const FVector InVelocity, const TArray<class UParticleModuleEventSendToGame*>& InEventData, const float InParticleTime);

    // void ReportEventCollision(const FName InEventName, const float InEmitterTime, const FVector InLocation,
    //     const FVector InDirection, const FVector InVelocity, const TArray<class UParticleModuleEventSendToGame*>& InEventData, 
    //     const float InParticleTime, const FVector InNormal, const float InTime, const int32 InItem, const FName InBoneName, UPhysicalMaterial* PhysMat);


    virtual void RewindEmitterInstances();
#pragma endregion

public:
    virtual void InitializeComponent() override;
    void CreateQuadTextureVertexBuffer();
    virtual void TickComponent(float DeltaTime) override;

    virtual int32 GetNumMaterials() const;
    virtual UMaterial* GetMaterial(int32 ElementIndex) const;
    virtual void SetMaterial(int32 ElementIndex, UMaterial* Material);
    
    // If particles have not already been initialised (ie. EmitterInstances created) do it now.
    virtual void InitParticles();
    virtual void UpdateDynamicData();
    void UpdateInstances(bool bEmptyInstances = false);
    void GenerateParticleEvent(const FName InEventName, const float InEmitterTime,
        const FVector InLocation, const FVector InDirection, const FVector InVelocity);
    
    void KillParticlesForced();

    void SpawnAllEmitters();
    void UpdateAllEmitters(float DeltaTime);

protected:
    static FDynamicEmitterDataBase* CreateDynamicDataFromReplay( FParticleEmitterInstance* EmitterInstance, const FDynamicEmitterReplayDataBase* EmitterReplayData, bool bSelected);
    
    void ClearDynamicData();

};