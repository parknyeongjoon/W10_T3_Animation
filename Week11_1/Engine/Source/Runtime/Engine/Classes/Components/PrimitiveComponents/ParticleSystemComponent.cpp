#include "ParticleSystemComponent.h"

#include "LaunchEngineLoop.h"
#include "QuadTexture.h"
#include "Paritcles/ParticleEmitterInstances.h"
#include "Particles/ParticleSystemWorldManager.h"
#include "Engine/Classes/Engine/World.h"
#include "Renderer/Renderer.h"
#include "UObject/Casts.h"

bool GIsAllowingParticles = true;

UParticleSystemComponent::UParticleSystemComponent()
{

}

void UParticleSystemComponent::TickComponent(float DeltaTime)
{
    UPrimitiveComponent::TickComponent(DeltaTime);
}

UMaterial* UParticleSystemComponent::GetMaterial(int32 ElementIndex) const
{
    // if (EmitterMaterials.IsValidIndex(ElementIndex) && EmitterMaterials[ElementIndex] != NULL)
    // {
    //     return EmitterMaterials[ElementIndex];
    // }
    // if (Template && Template->Emitters.IsValidIndex(ElementIndex))
    // {
    //     UParticleEmitter* Emitter = Template->Emitters[ElementIndex];
    //     if (Emitter && Emitter->LODLevels.Num() > 0)
    //     {
    //         UParticleLODLevel* EmitterLODLevel = Emitter->LODLevels[0];
    //         if (EmitterLODLevel && EmitterLODLevel->RequiredModule)
    //         {
    //             return EmitterLODLevel->RequiredModule->Material;
    //         }
    //     }
    // }
    return nullptr;
}

void UParticleSystemComponent::SetMaterial(int32 ElementIndex, UMaterial* Material)
{
    // if (Template && Template->Emitters.IsValidIndex(ElementIndex))
    // {
    //     if (!EmitterMaterials.IsValidIndex(ElementIndex))
    //     {
    //         EmitterMaterials.AddZeroed(ElementIndex + 1 - EmitterMaterials.Num());
    //     }
    //     EmitterMaterials[ElementIndex] = Material;
    //     bIsViewRelevanceDirty = true;
    // }
    // for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); ++EmitterIndex)
    // {
    //     if (FParticleEmitterInstance* Inst = EmitterInstances[EmitterIndex])
    //     {
    //         Inst->Tick_MaterialOverrides(EmitterIndex);
    //     }
    // }
}

FDynamicEmitterDataBase* UParticleSystemComponent::CreateDynamicDataFromReplay(FParticleEmitterInstance* EmitterInstance,
    const FDynamicEmitterReplayDataBase* EmitterReplayData, bool bSelected)
{
    switch( EmitterReplayData->eEmitterType )
    {
    case DET_Unknown:
        break;
    case DET_Sprite:
        break;
    case DET_Mesh:
        break;
    case DET_Beam2:
        break;
    case DET_Ribbon:
        break;
    case DET_AnimTrail:
        break;
    case DET_Custom:
        break;
    }

    return nullptr;
}

void UParticleSystemComponent::UpdateDynamicData()
{
}

void UParticleSystemComponent::UpdateInstances(bool bEmptyInstances)
{
    if (IsRegistered())
    {
        ResetParticles(bEmptyInstances);

        InitializeSystem();
        ActivateSystem();

        if (Template)
        {
            //UpdateComponentToWorld();
        }
    }
}

#pragma region Hide
FParticleSystemWorldManager* UParticleSystemComponent::GetWorldManager() const
{
    return FParticleSystemWorldManager::Get(GetWorld());
}

bool UParticleSystemComponent::ShouldActivate() const
{
    return (!IsActive() || (bWasDeactivated || bWasCompleted));
}

int32 UParticleSystemComponent::GetNumMaterials() const
{
    // if (Template)
    // {
    //     return Template->Emitters.Num();
    // }
    return 0;
}

void UParticleSystemComponent::Activate(bool bReset)
{
    if (Template != nullptr)
    {
        bDeactivateTriggered = false;

        if (bReset || ShouldActivate())
        {
            ActivateSystem(bReset);
        }
    }
}

void UParticleSystemComponent::Deactivate()
{
    if (ShouldActivate()==false)
    {
        DeactivateSystem();
    }
}

void UParticleSystemComponent::Complete()
{
    ResetParticles();
    SetComponentTickEnabled(false);
    if (bAutoDestroy)
    {
        DestroyComponent();
    }
}

void UParticleSystemComponent::DeactivateImmediate()
{
    Complete();
}

void UParticleSystemComponent::ActivateSystem(bool bFlagAsJustAttached)
{
    if (Template != nullptr)
    {
        return;
    }

    bOldPositionValid = false;
    OldPosition = FVector::ZeroVector;
    PartSysVelocity = FVector::ZeroVector;

    UWorld* world = GetWorld();

    if (Template)
    {
        if (EmitterInstances.Num() > 0)
        {
            int32 LiveCount = 0;

            for (int32 EmitterIndex =0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
            {
                const FParticleEmitterInstance* EmitInst = EmitterInstances[EmitterIndex];
                if (EmitInst)
                {
                    LiveCount += EmitInst->ActiveParticles;
                }
            }

            if (LiveCount > 0)
            {
                UE_LOG(LogLevel::Warning, TEXT("ActivateSystem called on PSysComp w/ live particles - %5d, %s"), LiveCount, *(Template->GetName()));
            }
        }
    }

    if (GIsAllowingParticles && Template)
    {
        const bool bWasAutoAttached = bDidAutoAttach;
        bDidAutoAttach = false;
        if (bAutoManageAttachment && world->WorldType == EWorldType::Game)
        {
            USceneComponent* NewParent = AutoAttachParent;
            if (NewParent)
            {
                const bool bAlreadyAttached = GetAttachParent() && (GetAttachParent() == NewParent) && GetAttachParent()->GetAttachChildren().Contains(this);
                if (!bAlreadyAttached)
                {
                    bDidAutoAttach = bWasAutoAttached;
                    SavedAutoAttachRelativeLocation = GetRelativeLocation();
                    SavedAutoAttachRelativeRotation = GetRelativeRotation();
                    SavedAutoAttachRelativeScale3D = GetRelativeScale();
                    AttachToComponent(NewParent);
                }

                bDidAutoAttach = true;
            }
        }
        
        AccumTickTime = 0.0;

        // Stop suppressing particle spawning.
        bSuppressSpawning = false;
        bool bNeedToUpdateTransform = bWasDeactivated;
        bWasCompleted = false;
        bWasDeactivated = false;
        bWasActive = false; // Set to false now, it may get set to true when it's deactivated due to unregister
        SetComponentTickEnabled(true);

        // if no instances, or recycling
        if (EmitterInstances.Num() == 0 || (world->WorldType == EWorldType::Game && (bHasBeenActivated)))
        {
            InitializeSystem();
        }
        else if (EmitterInstances.Num() > 0 && !(world->WorldType == EWorldType::Game))
        {
            // If currently running, re-activating rewinds the emitter to the start. Existing particles should stick around.
            for (int32 i=0; i<EmitterInstances.Num(); i++)
            {
                if (EmitterInstances[i])
                {
                    //EmitterInstances[i]->Rewind();
                    //EmitterInstances[i]->SetHaltSpawning(false);
                    //EmitterInstances[i]->SetHaltSpawningExternal(false);
                }
            }
        }

        // Flag the system as having been activated at least once
        bHasBeenActivated = true;
        
        // Clear tick time
        TimeSinceLastTick = 0;

        if (WarmupTime != 0.0f)
        {
            bool bSaveSkipUpdate = bSkipUpdateDynamicDataDuringTick;
            bSkipUpdateDynamicDataDuringTick = true;
            bWarmingUp = true;
            ResetBurstLists();

            float WarmupElapsed = 0.f;
            float WarmupTimestep = 0.032f;
            if (WarmupTickRate > 0)
            {
                WarmupTimestep = (WarmupTickRate <= WarmupTime) ? WarmupTickRate : WarmupTime;
            }

            while (WarmupElapsed < WarmupTime)
            {
                TickComponent(WarmupTimestep);
                WarmupElapsed += WarmupTimestep;
            }

            bWarmingUp = false;
            WarmupTime = 0.0f;
            bSkipUpdateDynamicDataDuringTick = bSaveSkipUpdate;
        }
    }
}

void UParticleSystemComponent::DeactivateSystem()
{
    UWorld* World = GetWorld();

    //We have seen some edge case where the world can be null here so avoid the crash and try to leave the component in a decent state until we can fix the underlying issue.
    if (World == nullptr)
    {
        UE_LOG(LogLevel::Error, TEXT("DeactivateSystem called on PSC with null World ptr! %s"), Template != NULL ? *Template->GetName() : TEXT("NULL"));

        ResetParticles(true);
        bDeactivateTriggered = false;
        bSuppressSpawning = true;
        bWasDeactivated = true;
        return;
    }

    bDeactivateTriggered = false;
    bSuppressSpawning = true;
    bWasDeactivated = true;

    bool bShouldMarkRenderStateDirty = false;
    for (int32 i = 0; i < EmitterInstances.Num(); i++)
    {
        FParticleEmitterInstance* Instance = EmitterInstances[i];
        if (Instance)
        {
//             if (Instance->bKillOnDeactivate)
//             {
//                 Instance->PreDestructorCall();
//                 // clean up other instances that may point to this one
//                 for (int32 InnerIndex=0; InnerIndex < EmitterInstances.Num(); InnerIndex++)
//                 {
//                     if (InnerIndex != i && EmitterInstances[InnerIndex] != NULL)
//                     {
//                         EmitterInstances[InnerIndex]->OnEmitterInstanceKilled(Instance);
//                     }
//                 }
//                 delete Instance;
//                 EmitterInstances[i] = NULL;
//                 bShouldMarkRenderStateDirty = true;
//             }
//             else
//             {
//                 Instance->OnDeactivateSystem();
//             }
        }
    }

    SetComponentTickEnabled(true);
}

void UParticleSystemComponent::ClearDynamicData()
{

}

void UParticleSystemComponent::InitParticles()
{
    if (Template != nullptr)
    {
        // WarmupTime = Template->WarmupTime;
        // WarmupTickRate = Template->WarmupTickRate;

        bIsViewRelevanceDirty = true;

        int32 NumInstances = EmitterInstances.Num();
        //int32 NumEmitters = Template->Emitters.Num();

        const bool bIsFirstCreate = NumInstances == 0;
        //EmitterInstances.SetNumZeroed(NumEmitters);

        bWasCompleted = bIsFirstCreate ? false : bWasCompleted;

        bool bClearDynamicData = false;

        //for (int32 Idx = 0; Idx < NumEmitters; Idx++)
        ///{
            // UParticleEmitter* Emitter = Template->Emitters[Idx];
            // if (Emitter)
            // {
            //     FParticleEmitterInstance* Instance = NumInstances == 0 ? NULL : EmitterInstances[Idx];
            //
            //     if (bShouldCreateAndOrInit)
            //     {
            //         if (Instance)
            //         {
            //             Instance->SetHaltSpawning(false);
            //             Instance->SetHaltSpawningExternal(false);
            //         }
            //         else
            //         {
            //             Instance = Emitter->CreateInstance(this);
            //             EmitterInstances[Idx] = Instance;
            //         }
            //
            //         if (Instance)
            //         {
            //             Instance->bEnabled = true;
            //             Instance->InitParameters(Emitter, this);
            //             Instance->Init();
            //
            //             PreferredLODLevel = FMath::Min(PreferredLODLevel, Emitter->LODLevels.Num());
            //             bSetLodLevels |= !bIsFirstCreate;//Only set lod levels if we init any instances and it's not the first creation time.
            //         }
            //     }
            //     else
            //     {
            //         if (Instance)
            //         {
            //             Instance->PreDestructorCall();
            //             delete Instance;
            //             EmitterInstances[Idx] = NULL;
            //             bClearDynamicData = true;
            //         }
            //     }
            // }
        //}
        
        // if (bClearDynamicData)
        // {
        //     ClearDynamicData();
        // }
    }
}

void UParticleSystemComponent::ResetParticles(bool bEmptyInstances)
{
    UWorld* OwningWorld = GetWorld();
    
    // Remove instances from scene.
    for( int32 InstanceIndex=0; InstanceIndex<EmitterInstances.Num(); InstanceIndex++ )
    {
        FParticleEmitterInstance* EmitterInstance = EmitterInstances[InstanceIndex];
        if( EmitterInstance )
        {
            if (!(!OwningWorld->WorldType == EWorldType::Game || bEmptyInstances))
            {
                EmitterInstance->SpriteTemplate	= nullptr;
                EmitterInstance->Component= nullptr;
            }
        }
    }

    // Remove instances if we're not running gameplay.ww
    if (!OwningWorld->WorldType == EWorldType::Game || bEmptyInstances)
    {
        for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
        {
            FParticleEmitterInstance* EmitInst = EmitterInstances[EmitterIndex];
            if (EmitInst)
            {
                //EmitInst->PreDestructorCall();
                delete EmitInst;
                EmitterInstances[EmitterIndex] = nullptr;
            }
        }
        EmitterInstances.Empty();
        ClearDynamicData();
    }
    else
    {
        for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
        {
            FParticleEmitterInstance* EmitInst = EmitterInstances[EmitterIndex];
            if (EmitInst)
            {
                //EmitInst->Rewind();
            }
        }
    }
}

void UParticleSystemComponent::ResetBurstLists()
{
    for (int32 i=0; i<EmitterInstances.Num(); i++)
    {
        if (EmitterInstances[i])
        {
            //EmitterInstances[i]->ResetBurstList();
        }
    }
}

bool UParticleSystemComponent::HasCompleted()
{
    bool bHasCompleted = true;
    bool bCanBeDeactivated = true;

    bool bClearDynamicData = false;
    for (int32 i=0; i<EmitterInstances.Num(); i++)
    {
        FParticleEmitterInstance* Instance = EmitterInstances[i];

        if (Instance && Instance->CurrentLODLevel /*&& Instance->bEnabled*/)
        {
//             if (!Instance->bEmitterIsDone)
//             {
//                 bCanBeDeactivated = false;
//             }
//
//             if (Instance->CurrentLODLevel->bEnabled)
//             {
//                 if (Instance->CurrentLODLevel->RequiredModule->EmitterLoops > 0)
//                 {
//                     if (bWasDeactivated && bSuppressSpawning)
//                     {
//                         if (Instance->ActiveParticles != 0)
//                         {
//                             bHasCompleted = false;
//                         }
//                     }
//                     else
//                     {
//                         if (Instance->HasCompleted())
//                         {
//                             if (Instance->bKillOnCompleted)
//                             {
//                                 Instance->PreDestructorCall();
//                                 // clean up other instances that may point to this one
//                                 for (int32 InnerIndex=0; InnerIndex < EmitterInstances.Num(); InnerIndex++)
//                                 {
//                                     if (InnerIndex != i && EmitterInstances[InnerIndex] != NULL)
//                                     {
//                                         EmitterInstances[InnerIndex]->OnEmitterInstanceKilled(Instance);
//                                     }
//                                 }
//                                 delete Instance;
//                                 EmitterInstances[i] = NULL;
//                                 bClearDynamicData = true;
//                             }
//                         }
//                         else
//                         {
//                             bHasCompleted = false;
//                         }
//                     }
//                 }
//                 else
//                 {
//                     if (bWasDeactivated)
//                     {
//                         if (Instance->ActiveParticles != 0)
//                         {
//                             bHasCompleted = false;
//                         }
//                     }
//                     else
//                     {
//                         bHasCompleted = false;
//                     }
//                 }
            }
            else
            {
                // UParticleEmitter* Em = Cast<UParticleEmitter>(Instance->CurrentLODLevel->GetOuter());
                // if (Em && Em->bDisabledLODsKeepEmitterAlive)
                // {
                //     bHasCompleted = false;
                // }				
            }
        }
    
    if (bCanBeDeactivated && Template /* && Template->bAutoDeactivate*/)
    {
        DeactivateSystem();
    }

    if (bClearDynamicData)
    {
        ClearDynamicData();
    }
	    
    return bHasCompleted;
}

void UParticleSystemComponent::InitializeSystem()
{
    if( GIsAllowingParticles)
    {
        if( Template != NULL )
        {
            //EmitterDelay = Template->Delay;

            // if( Template->bUseDelayRange )
            // {
            //     const float	Rand = RandomStream.FRand();
            //     EmitterDelay	 = Template->DelayLow + ((Template->Delay - Template->DelayLow) * Rand);
            // }
        }

        // Allocate the emitter instances and particle data
        InitParticles();
        // if (IsRegistered())
        // {
        //     AccumTickTime = 0.0;
        //     if ((IsActive() == false) && (bAutoActivate == true) && (bWasDeactivated == false))
        //     {
        //         SetActive(true);
        //     }
        // }
    }
}

void UParticleSystemComponent::ReportEventSpawn(const FName InEventName, const float InEmitterTime, const FVector InLocation,
    const FVector InVelocity, const TArray<class UParticleModuleEventSendToGame*>& InEventData)
{
    FParticleEventSpawnData* SpawnData = new(SpawnEvents)FParticleEventSpawnData;
    SpawnData->Type = EPET_Spawn;
    SpawnData->EventName = InEventName;
    SpawnData->EmitterTime = InEmitterTime;
    SpawnData->Location = InLocation;
    SpawnData->Velocity = InVelocity;
    SpawnData->EventData = InEventData;
}

void UParticleSystemComponent::ReportEventDeath(const FName InEventName, const float InEmitterTime, const FVector InLocation,
    const FVector InVelocity, const TArray<class UParticleModuleEventSendToGame*>& InEventData, const float InParticleTime)
{
    FParticleEventDeathData* DeathData = new(DeathEvents)FParticleEventDeathData;
    DeathData->Type = EPET_Death;
    DeathData->EventName = InEventName;
    DeathData->EmitterTime = InEmitterTime;
    DeathData->Location = InLocation;
    DeathData->Velocity = InVelocity;
    DeathData->EventData = InEventData;
    DeathData->ParticleTime = InParticleTime;
}

void UParticleSystemComponent::GenerateParticleEvent(const FName InEventName, const float InEmitterTime, const FVector InLocation,
    const FVector InDirection, const FVector InVelocity)
{
    FParticleEventKismetData* KismetData = new(KismetEvents)FParticleEventKismetData;
    KismetData->Type = EPET_Blueprint;
    KismetData->EventName = InEventName;
    KismetData->EmitterTime = InEmitterTime;
    KismetData->Location = InLocation;
    KismetData->Velocity = InVelocity;
}

void UParticleSystemComponent::KillParticlesForced()
{
    for (int32 EmitterIndex=0;EmitterIndex<EmitterInstances.Num();EmitterIndex++)
    {
        if (EmitterInstances[EmitterIndex])
        {
            //EmitterInstances[EmitterIndex]->KillParticlesForced();
        }
    }
}

void UParticleSystemComponent::RewindEmitterInstances()
{
    for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
    {
        FParticleEmitterInstance* EmitterInst = EmitterInstances[EmitterIndex];
        if (EmitterInst)
        {
            //EmitterInst->Rewind();
        }
    }
}
#pragma endregion 

void UParticleSystemComponent::InitializeComponent()
{
    UPrimitiveComponent::InitializeComponent();

    CreateQuadTextureVertexBuffer();
}

void UParticleSystemComponent::CreateQuadTextureVertexBuffer()
{
    FRenderer& Renderer = GEngineLoop.Renderer;
    FRenderResourceManager* ResourceManager = Renderer.GetResourceManager();
    
    if (ResourceManager->GetVertexBuffer(TEXT("QuadVB")) && ResourceManager->GetVertexBuffer(TEXT("QuadIB")))
    {
        VBIBTopologyMappingName = TEXT("Quad");
        return;
    }
    
    ID3D11Buffer* VB = ResourceManager->CreateImmutableVertexBuffer(quadTextureVertices, sizeof(quadTextureVertices));
    ResourceManager->AddOrSetVertexBuffer(TEXT("QuadVB"), VB);
    Renderer.MappingVBTopology(TEXT("Quad"), TEXT("QuadVB"), sizeof(FVertexTexture), 4);

    ID3D11Buffer* IB = ResourceManager->CreateIndexBuffer(quadTextureInices, sizeof(quadTextureInices) / sizeof(uint32));
    ResourceManager->AddOrSetIndexBuffer(TEXT("QuadIB"), IB);
    Renderer.MappingIB(TEXT("Quad"), TEXT("QuadIB"), sizeof(quadTextureInices) / sizeof(uint32));

    VBIBTopologyMappingName = TEXT("Quad");
}

