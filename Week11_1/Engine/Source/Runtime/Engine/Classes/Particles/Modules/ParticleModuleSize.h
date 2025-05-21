#pragma once

#include "ParticleModule.h"
#include <Distributions/DistributionVector.h>

struct FBaseParticle;

class UParticleModuleSize : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModule)
public:
    UParticleModuleSize() {}

    FRawDistributionVector StartSize;

    bool bUpdateSizeOverLife = false;

public:
    virtual void InitializeDefaults() override;

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    virtual EModuleType GetType() const override;

    // 파티클의 초기 크기를 설정하는 함수
    void SetInitialSize(const FVector& InSize);
    // 파티클의 초기 크기를 가져오는 함수
    FVector GetInitialSize() const;
};