#pragma once

#include "ParticleModuleTypeDataBase.h"
#include "Math/RandomStream.h"
#include "Components/Mesh/StaticMesh.h" // UPROPERTY 전방선언 못함

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