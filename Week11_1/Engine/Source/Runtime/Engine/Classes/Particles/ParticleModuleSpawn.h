#pragma once

#include "ParticleModule.h"

class UParticleModuleSpawn : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModule)
public:
    UParticleModuleSpawn() {}
    // 파티클의 생성 빈도를 설정하는 함수
    void SetSpawnRate(float InSpawnRate);
    // 파티클의 생성 빈도를 가져오는 함수
    float GetSpawnRate() const;
    // 파티클의 최대 수를 설정하는 함수
    void SetMaxParticles(int32 InMaxParticles);
    // 파티클의 최대 수를 가��오는 함수
    int32 GetMaxParticles() const;
};