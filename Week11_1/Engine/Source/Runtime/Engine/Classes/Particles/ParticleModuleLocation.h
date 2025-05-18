#pragma once

#include "ParticleModule.h"

class UParticleModuleLocation : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleLocation, UParticleModule)
public:
    UParticleModuleLocation() {}
    // 파티클의 초기 위치를 설정하는 함수
    void SetInitialLocation(const FVector& InLocation);
    // 파티클의 초기 위치를 가져오는 함수
    FVector GetInitialLocation() const;
};