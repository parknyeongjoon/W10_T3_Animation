#include "ParticleModuleRequired.h"
#include "Runtime/Engine/Particles/ParticleHelper.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include "LaunchEngineLoop.h"

UParticleModuleRequired::UParticleModuleRequired()
{
    // 초기값 지정
    Texture = FEngineLoop::ResourceManager.GetTexture(L"Contents/Textures/Particles/jewel_particle_yellow@2x.png").get();
    //Texture = FEngineLoop::ResourceManager.GetDefaultWhiteTexture().get();
    EmitterOrigin = FVector::ZeroVector;
    EmitterRotation = FRotator::ZeroRotator;

    EmitterDurationUseRange = false;
    EmitterDuration = 1.0f;
    EmitterDurationLow = 0.0f;
    EmitterDelayUseRange = false;

    SubImagesHorizontal = 1;
    SubImagesVertical = 1;
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
    //ReplayData->SubImageH = SubImagesHorizontal;
    //ReplayData->SubImageV = SubImagesVertical;
    ReplayData->EmitterOrigin = EmitterOrigin;
}

int32 UParticleModuleRequired::GetTotalSubImages() const
{
    return int32();
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
