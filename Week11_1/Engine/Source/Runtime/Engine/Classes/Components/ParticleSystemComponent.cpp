//#include "ParticleSystemComponent.h"
//#include "Runtime/Engine/Particles/ParticleEmitterInstance.h"
//#include <Particles/Modules/ParticleModule.h>
//#include "Engine/Classes/Particles/ParticleLODLevel.h"
//
//void UParticleSystemComponent::InitializeComponent()
//{
//}
//
//void UParticleSystemComponent::TickComponent(float DeltaTime)
//{
//}
//
//void UParticleSystemComponent::DestroyComponent()
//{
//}
//
//void UParticleSystemComponent::SpawnAllEmitters()
//{
//    if (!Template) return;
//
//    EmitterInstances.Empty();
//
//    for (UParticleEmitter* Emitter : Template->Emitters)
//    {
//        if (!Emitter) continue;
//
//        // 새 이미터 인스턴스를 생성
//        FParticleEmitterInstance* Instance;
//        Instance->SpriteTemplate = Emitter;
//        Instance->Component = this;
//        Instance->CurrentLODLevelIndex = 0;
//        Instance->CurrentLODLevel = Emitter->GetLODLevel(0); // 현재는 LOD0만 사용
//        Instance->Init(1024); // 임의 최대 파티클 수
//
//        // 파티클 모듈들의 초기화 및 첫 스폰 호출
//        if (Instance->CurrentLODLevel)
//        {
//            for (UParticleModule* Module : Instance->CurrentLODLevel->SpawnModules)
//            {
//                // TODO : SpawnTime, Interp 관련 시간 보간 추후 확인 필요. 
//                Module->Spawn(Instance, /*Offset*/ 0, /*SpawnTime*/ 0.0f, /*Interp*/ 1.0f);
//            }
//        }
//
//        EmitterInstances.Add(Instance);
//    }
//}
//
//void UParticleSystemComponent::UpdateAllEmitters(float DeltaTime)
//{
//    for (FParticleEmitterInstance* Instance : EmitterInstances)
//    {
//        if (!Instance || !Instance->CurrentLODLevel) {
//            UE_LOG(LogLevel::Warning, "There is no ParticleEmitter Instance or LOD Level");
//        }
//
//        // 파티클 라이프타임 체크 루프
//        for (int32 i = 0; i < Instance->ActiveParticles;) {
//            FBaseParticle* Particle = Instance->GetParticle(i);
//            
//            // 시간 경과
//            Particle->RelativeTime += DeltaTime;
//            
//            if (Particle->RelativeTime >= Particle->Lifetime) {
//                // 죽은 파티클 제거
//                Instance->KillParticle(i);
//            }
//            else {
//                ++i;
//            }
//        }
//
//        for (UParticleModule* Module : Instance->CurrentLODLevel->UpdateModules) {
//            Module->Update(Instance, /*offset*/ 0, DeltaTime);
//        }
//    }
//}
