#pragma once
#include "Container/Array.h"
#include "ParticleEmitter.h"
#include "Engine/Asset/Asset.h"

class UParticleSystemComponent;

class UParticleSystem : public UAsset
{
    DECLARE_CLASS(UParticleSystem, UAsset)
    UParticleSystem();
    ~UParticleSystem() override;
    
    bool LoadFromFile(const FString& filepath) override;

    bool SerializeToFile(std::ostream& Out) override;
    bool DeserializeFromFile(std::istream& In) override;
    
    uint32 MaxPoolSize;

    /**
     * 풀을 초기화하기 위해 미리 생성할 인스턴스 개수입니다.
     * 이 값만큼 로드 시점에 인스턴스를 채워 두어 런타임 활성화 비용을 분산할 수 있습니다.
     * 단, 레벨 로드 시점이 아닌 플레이 중 로드/언로드되는 시스템에 사용하면
     * 큰 히치(프레임 끊김)가 발생할 수 있으니 주의해야 합니다.
     */
    uint32 PoolPrimeSize = 0;

    
    /** UpdateTime_FPS - FixedTime 모드에서 초당 프레임 수(FPS)로 업데이트합니다 */
    float UpdateTime_FPS;

    /** UpdateTime_Delta - 내부용 변수입니다 */
    float UpdateTime_Delta;

    /**
     * WarmupTime - 파티클 시스템이 처음 렌더링될 때 예열할 시간입니다
     * 경고: WarmupTime은 활성화 시 요청된 시간만큼 파티클 시스템을 시뮬레이션함으로써 구현됩니다.
     * 특히 파티클 수가 많은 경우 버벅임(hitching)을 유발할 수 있으니 주의해서 사용하십시오.
     */
    float WarmupTime;

    /** WarmupTickRate - 예열 중 틱(tick)마다의 시간 간격입니다.
        값을 높이면 성능이 향상되고, 낮추면 정확도가 향상됩니다.
        0으로 설정하면 기본 틱 시간(tick time)을 사용합니다. */
    float WarmupTickRate;

    UPROPERTY(EditAnywhere, TArray<UParticleEmitter*>, Emitters, {};)

    /** Cascade에서 파티클 시스템을 미리보기 위해 사용하는 컴포넌트 */
    UParticleSystemComponent* PreviewComponent;

    /** 썸네일 이미지를 렌더링할 때 시스템을 배치할 거리 */
    float ThumbnailDistance;

    /** 썸네일 이미지를 위해 시스템을 예열할 시간 */
    float ThumbnailWarmup;

    /** ActivateSystem 호출 시 파티클 시스템이 지연될 시간 */
    float Delay;

    /** 지연 시간을 범위로 사용할 경우 지연 시간의 하한값 */
    float DelayLow;

    /** true이면, 시스템의 Z축이 카메라를 향하도록 정렬됨 */
    bool bOrientZAxisTowardCamera;

    /** 에디터 전용: Cascade에서 피크 활성 파티클 수를 초기화하길 원하는 경우 표시 */
    bool bShouldResetPeakCounts;

    /** 로드 시 설정됨. 해당 시스템이 물리를 사용하는지 여부 표시 */
    bool bHasPhysics;

    /**
     * true이면, 지연 시간을 [DelayLow..Delay] 범위에서 선택함
     */
    bool bUseDelayRange;

    /** 관리형 틱(Manged Ticking)을 허용할지 여부 */
    bool bAllowManagedTicking;

    /** 자동 비활성화 여부 */
    bool bAutoDeactivate;

private:
    /** 루프를 영원히 반복하는 이미터가 있는지 여부 */
    bool bAnyEmitterLoopsForever;

public:
    /** 두 틱 사이의 최소 시간 */
    uint32 MinTimeBetweenTicks;

    /** 모든 이미터가 중요하지 않게 된 후, 시스템이 반응하기까지의 지연 시간 */
    float InsignificanceDelay;

    /** 풀링 가능 여부 반환 */
    bool CanBePooled() const;

    /** 활성 파티클 최대 수 계산 */
    virtual bool CalculateMaxActiveParticleCounts();

    /**
     * 파티클 시스템의 스폰을 지연시킬 시간을 설정함
     */
    void SetDelay(float InDelay);

    /**
     * 중복된 모듈을 모두 제거함
     *
     * @param bInMarkForCooker true이면 제거된 오브젝트를 요리 대상에서 제외함
     * @param OutRemovedModules 선택적으로 제거된 모듈을 담을 맵
     *
     * @return 성공 여부
     */
    bool RemoveAllDuplicateModules(bool bInMarkForCooker, TMap<UObject*,bool>* OutRemovedModules);

    /** 모든 모듈 리스트를 갱신 */
    void UpdateAllModuleLists();

    /**
     * 파티클 시스템의 모든 이미터를 빌드함
     */
    void BuildEmitters();

    /**
     * 이 시스템에 주어진 타입의 이미터가 포함되어 있는지 반환
     * @param TypeData 확인할 이미터 타입 (UParticleModuleTypeDataBase의 자식 클래스여야 함)
     */
    bool ContainsEmitterType(UClass* TypeData);

    /** 무한 루프하는 이미터가 있는지 확인 */
    bool IsLooping() const { return bAnyEmitterLoopsForever; }

    /** 시스템이 불멸인지 여부 확인 (무한 루프 + 무제한 지속시간) */
    bool IsImmortal() const { return bIsImmortal; }

    bool AllowManagedTicking()const { return bAllowManagedTicking; }

private:
    /** 중요도 관리를 해야 하는지 여부 */
    bool bShouldManageSignificance;

    /** 어떤 이미터도 무한 루프 + 무한 지속시간으로 인해 결코 사라지지 않는 경우 true */
    bool bIsImmortal;

    /** 어떤 이미터가 좀비 상태(무한 루프하면서도 일정 시점 이후 스폰을 멈추는 경우)가 되는지 여부 */
    bool bWillBecomeZombie;

};