#pragma once

#include "ParticleModule.h"

class UParticleModuleVelocity : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleVelocity, UParticleModule)

public:
    UParticleModuleVelocity() {}
    // 파티클의 초기 속도를 설정하는 함수
    void SetInitialVelocity(const FVector& InVelocity);
    // 파티클의 초기 속도를 가져오는 함수
    FVector GetInitialVelocity() const;
};