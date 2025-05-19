#include "ParticleSystem.h"

#include "ParticleModule.h"

UParticleSystem::UParticleSystem()
    : Super()
    , bAnyEmitterLoopsForever(false)
    , bShouldManageSignificance(false)
    , bIsImmortal(false)
    , bWillBecomeZombie(false)
{
    ThumbnailDistance = 200.0;
    ThumbnailWarmup = 1.0;

    UpdateTime_FPS = 60.0f;
    UpdateTime_Delta = 1.0f/60.0f;
    WarmupTime = 0.0f;
    WarmupTickRate = 0.0f;
    
    bAutoDeactivate = true;
    MinTimeBetweenTicks = 0;
    InsignificanceDelay = 0.0f;
    MaxPoolSize = 32;

    bAllowManagedTicking = true;
}

bool UParticleSystem::CanBePooled()const
{
    if (MaxPoolSize == 0)
    {
        return false;
    }

    return true;
}

bool UParticleSystem::CalculateMaxActiveParticleCounts()
{
    const bool bSuccess = true;

    for (int32 EmitterIndex = 0; EmitterIndex < Emitters.Num(); EmitterIndex++)
    {
        const UParticleEmitter* Emitter = Emitters[EmitterIndex];
        if (Emitter)
        {
            // if (Emitter->CalculateMaxActiveParticleCount() == false)
            // {
            //     bSuccess = false;
            // }
        }
    }

    return bSuccess;
}

void UParticleSystem::SetDelay(const float InDelay)
{
    Delay = InDelay;
}

bool UParticleSystem::RemoveAllDuplicateModules(bool bInMarkForCooker, TMap<UObject*, bool>* OutRemovedModules)
{
    // 이 함수는 하나의 UParticleSystem 에셋을 스캔해서 같은 클래스에 속하고, 설정(프로퍼티)까지 완전히 동일한 파티클 모듈 인스턴스를 찾아내고, 중복된 모듈을 하나의 대표 모듈로 통합해 주는 작업을 합니다.
    //
    // 모듈 수집
        // 모든 이터미터(Emitters)와 각 LOD 레벨의 SpawnModule 및 Modules[]를 훑으면서
        // ClassToModulesMap 에 “클래스 → (모듈 인스턴스 → 개수)” 맵을 만듭니다.
    // 중복 검사
        // 같은 클래스 내에서 모듈들이 프로퍼티(Transient·EditorOnly·Cascade 카테고리 제외)까지 완벽히 동일한지 Property::Identical_InContainer 로 비교
        // 완전히 일치하는 모듈간에는 한쪽을 “대표 모듈”로 삼고, 나머지를 ReplaceModuleMap 에 등록
    // 중복 제거
        // 실제로 LOD 레벨 리스트를 다시 돌며, ReplaceModuleMap 에 등록된 모듈들은 모두 대표 모듈 포인터로 치환(replace)
        // bInMarkForCooker 가 켜져 있다면, 제거된 모듈을 RF_Transient 로 표시해 패키지 사이즈를 줄이도록 준비
    // 후처리
        // 에디터·쿠커 전용 옵션에 따라 필요하다면, 실제 리스트를 업데이트 (UpdateAllModuleLists)

    return true;
}

void UParticleSystem::UpdateAllModuleLists()
{
    // for (int32 EmitterIdx = 0; EmitterIdx < Emitters.Num(); EmitterIdx++)
    // {
    //     UParticleEmitter* Emitter = Emitters[EmitterIdx];
    //     if (Emitter != nullptr)
    //     {
    //         for (int32 LODIdx = 0; LODIdx < Emitter->LODLevels.Num(); LODIdx++)
    //         {
    //             UParticleLODLevel* LODLevel = Emitter->LODLevels[LODIdx];
    //             if (LODLevel != nullptr)
    //             {
    //                 //LOD 레벨별 모듈 리스트 갱신  
    //                 LODLevel->UpdateModuleLists();
    //             }
    //         }
    //
    //         // Allow type data module to cache any module info
    //         if(Emitter->LODLevels.Num() > 0)
    //         {
    //             UParticleLODLevel* HighLODLevel = Emitter->LODLevels[0];
    //             if (HighLODLevel != nullptr && HighLODLevel->TypeDataModule != nullptr)
    //             {
    //                 // Allow TypeData module to cache pointers to modules
    //                 HighLODLevel->TypeDataModule->CacheModuleInfo(Emitter);
    //             }
    //         }
    //
    //         // Update any cached info from modules on the emitter
    //         Emitter->CacheEmitterModuleInfo();
    //     }
    // }
}

void UParticleSystem::BuildEmitters()
{
    const int32 EmitterCount = Emitters.Num();
    for ( int32 EmitterIndex = 0; EmitterIndex < EmitterCount; ++EmitterIndex )
    {
        if (UParticleEmitter* Emitter = Emitters[EmitterIndex])
        {
            //Emitter->Build();
        }
    }
}

bool UParticleSystem::ContainsEmitterType(UClass* TypeData)
{
    for ( int32 EmitterIndex = 0; EmitterIndex < Emitters.Num(); ++EmitterIndex)
    {
        UParticleEmitter* Emitter = Emitters[EmitterIndex];
        if (Emitter)
        {
            // UParticleLODLevel* LODLevel = Emitter->LODLevels[0];
            // if (LODLevel && LODLevel->TypeDataModule && LODLevel->TypeDataModule->IsA(TypeData))
            // {
            //     return true;
            // }
        }
    }
	
    return false;
}
