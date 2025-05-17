#include "ParticleSystemWorldManager.h"

#include "Components/PrimitiveComponents/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "UserInterface/Console.h"

TMap<UWorld*, FParticleSystemWorldManager*> FParticleSystemWorldManager::WorldManagers = {};

void FParticleSystemWorldManager::OnWorldInit(UWorld* World)
{
    FParticleSystemWorldManager* NewWorldMan = new FParticleSystemWorldManager(World);
    WorldManagers.Add(World, NewWorldMan);
}

void FParticleSystemWorldManager::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
    if (FParticleSystemWorldManager** WorldMan = WorldManagers.Find(World))
    {
        UE_LOG(LogLevel::Warning, TEXT("| OnWorldCleanup | WorldMan: %p | World: %p | %s |"), *WorldMan, World, GetData(World->GetName()));
        delete (*WorldMan);
        WorldManagers.Remove(World);
    }
}

void FParticleSystemWorldManager::OnPreWorldFinishDestroy(UWorld* World)
{
    if (FParticleSystemWorldManager** WorldMan = WorldManagers.Find(World))
    {
        UE_LOG(LogLevel::Warning, TEXT("| OnPreWorldFinishDestroy | WorldMan: %p | World: %p | %s |"), *WorldMan, World, GetData(World->GetName()));
        delete (*WorldMan);
        WorldManagers.Remove(World);
    }
}

void FParticleSystemWorldManager::OnWorldBeginTearDown(UWorld* World)
{
    if (FParticleSystemWorldManager** WorldMan = WorldManagers.Find(World))
    {
        UE_LOG(LogLevel::Warning, TEXT("| OnWorldBeginTearDown | WorldMan: %p | World: %p | %s |"), *WorldMan, World, GetData(World->GetName()));
        delete (*WorldMan);
        WorldManagers.Remove(World);
    }
}

FParticleSystemWorldManager::FParticleSystemWorldManager(UWorld* InWorld)
{
}

bool FParticleSystemWorldManager::RegisterComponent(UParticleSystemComponent* PSC)
{
    std::lock_guard<std::mutex> lock(Mtx);
    if (!PendingRegisterPSCs.Contains(PSC))
    {
        PendingRegisterPSCs.Add(PSC);
        return true;
    }
    return false;
    
}

void FParticleSystemWorldManager::UnregisterComponent(UParticleSystemComponent* PSC)
{
    std::lock_guard<std::mutex> lock(Mtx);
    int32 Index;
    // 이미 관리 중이면 즉시 제거
    if (ManagedPSCs.Find(PSC, Index))
    {
        RemovePSC(Index);
    }
    else
    {
        // 등록 대기 중이면 취소
        PendingRegisterPSCs.Remove(PSC);
    }
}

void FParticleSystemWorldManager::Tick(const float DeltaTime, ELevelTick TickType)
{
    // 등록 대기 처리
    {
        std::lock_guard<std::mutex> lock(Mtx);
        for (auto* PSC : PendingRegisterPSCs)
        {
            AddPSC(PSC);
        }
        PendingRegisterPSCs.Empty();
    }

    // 모든 PSC 틱 실행
    for (auto* PSC : ManagedPSCs)
    {
        if (PSC)
        {
            PSC->TickComponent(DeltaTime);
        }
    }   
}

void FParticleSystemWorldManager::AddPSC(UParticleSystemComponent* PSC)
{
    ManagedPSCs.Add(PSC);
}

void FParticleSystemWorldManager::RemovePSC(const int32 PSCIndex)
{
    if (ManagedPSCs.IsValidIndex(PSCIndex))
    {
        ManagedPSCs.RemoveAt(PSCIndex);
    }
}
