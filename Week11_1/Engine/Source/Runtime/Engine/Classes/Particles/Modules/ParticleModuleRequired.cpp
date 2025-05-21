#include "ParticleModuleRequired.h"
#include "Runtime/Engine/Particles/ParticleHelper.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include "LaunchEngineLoop.h"
#include "ParticleModuleDefaults.h"
UParticleModuleRequired::UParticleModuleRequired()
{
    // 초기값 지정
    Texture = FEngineLoop::ResourceManager.GetTexture(ParticleModuleDefaults::Required::TexturePath).get();
    EmitterOrigin = ParticleModuleDefaults::Required::EmitterOrigin;
    // TODO : rotation 테스트 필요.
    EmitterRotation = ParticleModuleDefaults::Required::EmitterRotation;

    EmitterDurationUseRange = ParticleModuleDefaults::Required::bUseDurationRange;
    EmitterDuration = ParticleModuleDefaults::Required::EmitterDuration;
    EmitterDurationLow = ParticleModuleDefaults::Required::EmitterDurationLow;
    EmitterDelayUseRange = ParticleModuleDefaults::Required::bUseDelayRange;

    SubImagesHorizontal = ParticleModuleDefaults::Required::SubImagesHorizontal;
    SubImagesVertical = ParticleModuleDefaults::Required::SubImagesVertical;
}

void UParticleModuleRequired::InitializeDefaults()
{
    //if (!Texture) {
    //    //FIXME : 기본 머티리얼 지정.
    //}
}

void UParticleModuleRequired::CacheEmitterData(FParticleEmitterInstance* Instance) const
{   
    Instance->ParticleSize = sizeof(FBaseParticle);
    Instance->RequiredModule = const_cast<UParticleModuleRequired*>(this); // 단순 참조만 저장
}

void UParticleModuleRequired::FillReplayData(FDynamicSpriteEmitterReplayDataBase* ReplayData) const
{
    ReplayData->Texture = Texture; // FTexture* => UMaterialInterface*로 변경 필요
    ReplayData->SubImages_Horizontal = SubImagesHorizontal;
    ReplayData->SubImages_Vertical = SubImagesVertical;
    ReplayData->EmitterOrigin = EmitterOrigin;
}

int32 UParticleModuleRequired::GetTotalSubImages() const
{
    return SubImagesHorizontal * SubImagesVertical;
}

float UParticleModuleRequired::ComputeEmitterDuration(float RandomFactor) const
{
    return 0.0f;
}



EModuleType UParticleModuleRequired::GetType() const
{
    return EModuleType::Spawn;;
}

void UParticleModuleRequired::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    // 아무것도 안함
}

void UParticleModuleRequired::PostEditChangeProperty()
{
    if (SubImagesHorizontal <= 0) SubImagesHorizontal = 1;
    if (SubImagesVertical <= 0) SubImagesVertical = 1;
}
