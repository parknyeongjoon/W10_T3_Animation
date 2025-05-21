#pragma once
#include <mutex>

#include "EngineBaseTypes.h"
#include "Container/Array.h"
#include "Container/Map.h"

class UParticleSystemComponent;
class UWorld;

class FParticleSystemWorldManager
{
private:
    static TMap<UWorld*, FParticleSystemWorldManager*> WorldManagers;
    
public: 
    static void OnWorldInit(UWorld* InWorld);
    static void OnWorldCleanup(UWorld* InWorld);
    static void OnPreWorldFinishDestroy(UWorld* World);
    static void OnWorldBeginTearDown(UWorld* InWorld);

    FORCEINLINE static FParticleSystemWorldManager* Get(UWorld* World)
    {
        FParticleSystemWorldManager** Ret =  WorldManagers.Find(World);
        if (Ret)
        {
            return *Ret;
        }
        return nullptr;
    }
public:
    FParticleSystemWorldManager(UWorld* InWorld);
    ~FParticleSystemWorldManager();
    void Cleanup();

    bool RegisterComponent(UParticleSystemComponent* PSC);
    void UnregisterComponent(UParticleSystemComponent* PSC);

    void Tick(float DeltaTime, ELevelTick TickType);

private:

    void AddPSC(UParticleSystemComponent* PSC);
    void RemovePSC(int32 PSCIndex);
    
    UWorld* World;

    int32 bCachedParticleWorldManagerEnabled;

    TArray<UParticleSystemComponent*> ManagedPSCs;
    
    TArray<UParticleSystemComponent*> PendingRegisterPSCs;

    std::mutex Mtx; // 스레드 안정성
};

extern int32 GbEnablePSCWorldManager;
