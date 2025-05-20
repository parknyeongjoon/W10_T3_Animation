#pragma once

#include "ParticleModuleTypeDataBase.h"
#include "Math/RandomStream.h"

class UStaticMesh;

class UParticleModuleTypeDataMesh : public UParticleModuleTypeDataBase
{
    DECLARE_CLASS(UParticleModuleTypeDataMesh, UParticleModuleTypeDataBase)
public:
    UParticleModuleTypeDataMesh() = default;

    virtual FParticleEmitterInstance* CreateInstance(
        UParticleEmitter* InEmitterParent,
        UParticleSystemComponent* InComponent) override;

    virtual bool IsAMeshEmitter() const override { return true; }

    UPROPERTY(
        EditAnywhere,
        UStaticMesh*,
        Mesh,
        = nullptr
    )

    FRandomStream RandomStream;
};