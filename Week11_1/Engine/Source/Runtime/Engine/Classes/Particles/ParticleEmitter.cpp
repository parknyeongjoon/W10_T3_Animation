#include "ParticleEmitter.h"
#include <UserInterface/Console.h>
#include "Components/PrimitiveComponents/ParticleSystemComponent.h"
#include "Engine/FLoaderOBJ.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Engine/Classes/Particles/ParticleLODLevel.h"
#include "Particles/TypeData/ParticleModuleTypeDataBase.h"
#include "Particles//Modules/ParticleModuleRequired.h"
#include "Particles//Modules/ParticleModuleSpawn.h"

// 여기로 들어오면 안됨
FParticleEmitterInstance* UParticleEmitter::CreateInstance(UParticleSystemComponent* InComponent)
{
    UE_LOG(LogLevel::Error, "UParticleEmitter::CreateInstance is pure virtual");
    assert(0); // UParticleSpriteEmitter로 해야함
    return nullptr;
}

UParticleLODLevel* UParticleEmitter::GetLODLevel(int level)
{
    if (level >= 0 && level < LODLevels.Num())
    {
        return LODLevels[level];
    }
    else {
        UE_LOG(LogLevel::Warning, "Invalid LodLevels Index");
        return nullptr;
    }
}

// FParticleEmitterInstance를 생성하는 함수
// UParticleModuleTypeDataBase가 존재하면 그에 맞춰서 생성함.
// 없을경우 스프라이트 생성
FParticleEmitterInstance* UParticleSpriteEmitter::CreateInstance(UParticleSystemComponent* InComponent)
{
    FParticleEmitterInstance* Instance = nullptr;

    UParticleLODLevel* LODLevel = GetLODLevel(0);

    // 없으면 생성해줌
    if (!LODLevel)
    {
        LODLevel = FObjectFactory::ConstructObject<UParticleLODLevel>(this);
        
        UParticleModuleRequired* RequiredModule = FObjectFactory::ConstructObject<UParticleModuleRequired>(LODLevel);
        LODLevel->RequiredModule = RequiredModule;
        LODLevel->Modules.Add(RequiredModule);

        UParticleModuleSpawn* SpawnModule = FObjectFactory::ConstructObject<UParticleModuleSpawn>(LODLevel);
        LODLevel->Modules.Add(SpawnModule);
        LODLevel->AnalyzeModules();
    }
    // 만약 UParticleModuleTypeDataBase의 자식 클래스가 있으면 (= mesh, ribbon 등)
    // 그 모듈에 맞춰서 인스턴스를 생성한다.
    if (LODLevel->TypeDataModule)
    {
        Instance = LODLevel->TypeDataModule->CreateInstance(this, InComponent);
        LODLevel->RequiredModule->Mesh = FManagerOBJ::CreateStaticMesh("Assets/Dodge/Dodge.obj");

    }
    // 없을경우 스프라이트 생성
    else
    {
        Instance = new FParticleSpriteEmitterInstance(); // sprite가 기본값
        Instance->InitParameters(this, InComponent);
    }

    if (Instance)
    {
        Instance->CurrentLODLevelIndex = 0;
        Instance->CurrentLODLevel = LODLevel; // LOD는 0만 사용
        Instance->CurrentLODLevel->AnalyzeModules();
        Instance->RequiredModule = Instance->CurrentLODLevel->RequiredModule;
        Instance->Init(1024); // TODO : 임시. 값을 정하게 해줘야함.
    }

    return Instance;
}

