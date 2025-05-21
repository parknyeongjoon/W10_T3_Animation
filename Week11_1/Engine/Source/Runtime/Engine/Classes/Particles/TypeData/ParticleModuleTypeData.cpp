#include "ParticleModuleTypeDataMesh.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Particles/ParticleEmitter.h"
#include "Engine/Classes/Particles/ParticleLODLevel.h"

FParticleEmitterInstance* UParticleModuleTypeDataBase::CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent)
{
    return nullptr;
}

// UParticleSpriteEmitter::CreateInstance()로부터 호출됨
// 현재 UParticleSystemComponent::SpawnAllEmitters()와 겹침. 해결 방법 찾야야함.
FParticleEmitterInstance* UParticleModuleTypeDataMesh::CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent)
{
    FParticleMeshEmitterInstance* Instance = new FParticleMeshEmitterInstance();
    Instance->CurrentLODLevelIndex = 0;
    Instance->CurrentLODLevel = InEmitterParent->GetLODLevel(0);
    Instance->CurrentLODLevel->AnalyzeModules();
    Instance->RequiredModule = Instance->CurrentLODLevel->RequiredModule;
    Instance->MeshTypeData = this;
    Instance->InitParameters(InEmitterParent, InComponent);

    Instance->Init(64); // TODO : 임시. 값을 정하게 해줘야함.

    return Instance;
}