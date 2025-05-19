#pragma once
#include "Container/Array.h"
#include "Particles/ParticleSystem.h"
#include "Components/SceneComponent.h"

class FDynamicEmitterDataBase;

class UParticleSystemComponent : public USceneComponent
{
    DECLARE_CLASS(UParticleSystemComponent, USceneComponent)
public:
    UParticleSystemComponent() = default;
    UParticleSystem* Template;

    TArray<struct FParticleEmitterInstance*> EmitterInstances;

    TArray<FDynamicEmitterDataBase*> EmitterRenderData;
public:
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    void DestroyComponent() override;

    void SpawnAllEmitters();
    void UpdateAllEmitters(float DeltaTime);

};