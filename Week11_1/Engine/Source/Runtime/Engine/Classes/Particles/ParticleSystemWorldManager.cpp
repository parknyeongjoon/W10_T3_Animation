#include "ParticleSystemWorldManager.h"

#include "ParticleSystem.h"
#include "Components/PrimitiveComponents/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "UserInterface/Console.h"

TMap<UWorld*, FParticleSystemWorldManager*> FParticleSystemWorldManager::WorldManagers = {};

int32 GbEnablePSCWorldManager = 1;

void FParticleSystemWorldManager::OnWorldInit(UWorld* InWorld)
{
    FParticleSystemWorldManager* NewWorldMan = new FParticleSystemWorldManager(InWorld);
    WorldManagers.Add(InWorld, NewWorldMan);
}

void FParticleSystemWorldManager::OnWorldCleanup(UWorld* InWorld)
{
    if (FParticleSystemWorldManager** WorldMan = WorldManagers.Find(InWorld))
    {
        UE_LOG(LogLevel::Warning, TEXT("| OnWorldCleanup | WorldMan: %p | World: %p | %s |"), *WorldMan, InWorld, GetData(InWorld->GetName()));
        delete (*WorldMan);
        WorldManagers.Remove(InWorld);
    }
}

void FParticleSystemWorldManager::OnPreWorldFinishDestroy(UWorld* InWorld)
{
    if (FParticleSystemWorldManager** WorldMan = WorldManagers.Find(InWorld))
    {
        UE_LOG(LogLevel::Warning, TEXT("| OnPreWorldFinishDestroy | WorldMan: %p | World: %p | %s |"), *WorldMan, InWorld, GetData(InWorld->GetName()));
        delete (*WorldMan);
        WorldManagers.Remove(InWorld);
    }
}

void FParticleSystemWorldManager::OnWorldBeginTearDown(UWorld* InWorld)
{
    if (FParticleSystemWorldManager** WorldMan = WorldManagers.Find(InWorld))
    {
        UE_LOG(LogLevel::Warning, TEXT("| OnWorldBeginTearDown | WorldMan: %p | World: %p | %s |"), *WorldMan, InWorld, GetData(InWorld->GetName()));
        delete (*WorldMan);
        WorldManagers.Remove(InWorld);
    }
}

FParticleSystemWorldManager::FParticleSystemWorldManager(UWorld* InWorld)
{
    bCachedParticleWorldManagerEnabled = GbEnablePSCWorldManager;
    World = InWorld;
}

FParticleSystemWorldManager::~FParticleSystemWorldManager()
{
    Cleanup();
}

void FParticleSystemWorldManager::Cleanup()
{
    // Clear out pending particle system components.
    for (UParticleSystemComponent* PendingRegisterPSC : PendingRegisterPSCs)
    {
        if (PendingRegisterPSC != nullptr)
        {
            PendingRegisterPSC->SetManagerHandle(INDEX_NONE);
            PendingRegisterPSC->SetPendingManagerAdd(false);
        }
    }
    PendingRegisterPSCs.Empty();
    
    // Clear out actively managed particle system components.
    for (int32 PSCIndex = ManagedPSCs.Num() - 1; PSCIndex >= 0; --PSCIndex)
    {
        RemovePSC(PSCIndex);
    }

    World = nullptr;
}

bool FParticleSystemWorldManager::RegisterComponent(UParticleSystemComponent* PSC)
{
    std::lock_guard<std::mutex> lock(Mtx);
    int32 Handle = PSC->GetManagerHandle();
    if (Handle == INDEX_NONE)
    {
        if (!PSC->IsPendingManagerAdd())
        {
            Handle = PendingRegisterPSCs.Add(PSC);
            PSC->SetManagerHandle(Handle);
            PSC->SetPendingManagerAdd(true);

            UE_LOG(LogLevel::Warning, TEXT("| Register New: %p | Man: %p | %d | %s"), PSC, this, Handle, *PSC->Template->GetName());
            return true;
        }
        else
        {
            UE_LOG(LogLevel::Warning, TEXT("| Register Existing Pending PSC: %p | Man: %p | %d | %s"), PSC, this, Handle, *PSC->Template->GetName());
            return false;
        }
    }

    return true;
    
}

void FParticleSystemWorldManager::UnregisterComponent(UParticleSystemComponent* PSC)
{
    std::lock_guard<std::mutex> lock(Mtx);
    int32 Handle = PSC->GetManagerHandle();
    if (Handle != INDEX_NONE)
    {
        if (PSC->IsPendingManagerAdd())
        {
            UE_LOG(LogLevel::Warning, TEXT("| UnRegister Pending PSC: %p | Man: %p | %d | %s"), PSC, this, Handle, *PSC->Template->GetName());

            //Clear existing handle
            if (PendingRegisterPSCs[Handle])
            {
                PendingRegisterPSCs[Handle]->SetManagerHandle(INDEX_NONE);
            }
            else
            {
                // Handle scenario where registration and destruction of a component happens 
                // without FParticleSystemWorldManager tick in between and component being nulled
                // after being marked as PendingKill
                PSC->SetManagerHandle(INDEX_NONE);
            }

            PendingRegisterPSCs.RemoveAtSwap(Handle);

            //Update handle for moved PCS.
            if (PendingRegisterPSCs.IsValidIndex(Handle))
            {
                PendingRegisterPSCs[Handle]->SetManagerHandle(Handle);
            }

            PSC->SetPendingManagerAdd(false);
        }
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
    if (PSC != nullptr)
    {
        int32 Handle = ManagedPSCs.Add(PSC);
        PSC->SetManagerHandle(Handle);
        PSC->SetPendingManagerAdd(false);
    }
}

void FParticleSystemWorldManager::RemovePSC(const int32 PSCIndex)
{
    UParticleSystemComponent* PSC = ManagedPSCs[PSCIndex];
    
    if (PSC)
    {
        PSC->SetManagerHandle(INDEX_NONE);
        PSC->SetPendingManagerRemove(false);
    }

    ManagedPSCs.RemoveAtSwap(PSCIndex);

    if (ManagedPSCs.IsValidIndex(PSCIndex))
    {
        if (ManagedPSCs[PSCIndex])
        {
            ManagedPSCs[PSCIndex]->SetManagerHandle(PSCIndex);
        }
    }
}
