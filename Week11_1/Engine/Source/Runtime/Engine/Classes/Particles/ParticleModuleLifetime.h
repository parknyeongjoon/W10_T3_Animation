#pragma once

#include "ParticleModule.h"

class UParticleModuleLifetime : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleLifetime, UParticleModule)
public:
    UParticleModuleLifetime() {}
    // 파티클의 수명을 설정하는 함수
    void SetLifetime(float InLifetime);
    // 파티클의 수명을 가져오는 함수
    float GetLifetime() const;
};