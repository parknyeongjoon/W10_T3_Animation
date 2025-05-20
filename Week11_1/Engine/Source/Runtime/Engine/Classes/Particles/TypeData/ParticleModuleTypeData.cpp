#include "ParticleModuleTypeDataMesh.h"
#include "Particles/ParticleEmitterInstances.h"

FParticleEmitterInstance* UParticleModuleTypeDataBase::CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent)
{
    return nullptr;
}

// 현재 UParticleSystemComponent::SpawnAllEmitters()와 겹침. 해결 방법 찾야야함.
FParticleEmitterInstance* UParticleModuleTypeDataMesh::CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent)
{
    FParticleEmitterInstance* Instance = new FParticleEmitterInstance();

    Instance->Init(64); // TODO : 임시. 값을 정하게 해줘야함.

    return Instance;
}