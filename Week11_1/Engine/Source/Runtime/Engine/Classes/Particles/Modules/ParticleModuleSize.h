#pragma once

#include "ParticleModule.h"

class UParticleModuleSize : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModule)
public:
    UParticleModuleSize() {}
    // 파티클의 초기 크기를 설정하는 함수
    void SetInitialSize(const FVector& InSize);
    // 파티클의 초기 크기를 가져오는 함수
    FVector GetInitialSize() const;
};