#pragma once

#pragma once
#include "Core/HAL/PlatformType.h"
#include "ParticleHelper.h"

class UParticleEmitter;
class UParticleSystemComponent;
class UParticleLODLevel;

struct FParticleEmitterInstance
{
    UParticleEmitter* SpriteTemplate;

    // Owner
    UParticleSystemComponent* Component;

    int32 CurrentLODLevelIndex;
    UParticleLODLevel* CurrentLODLevel;

    /** Pointer to the particle data array.                             */
    uint8* ParticleData;
    /** Pointer to the particle index array.                            */
    uint16* ParticleIndices;
    /** Pointer to the instance data array.                             */
    uint8* InstanceData;
    /** The size of the Instance data array.                            */
    int32 InstancePayloadSize;
    /** The offset to the particle data.                                */
    int32 PayloadOffset;
    /** The total size of a particle (in bytes).                        */
    int32 ParticleSize;
    /** The stride between particles in the ParticleData array.         */
    int32 ParticleStride;
    /** The number of particles currently active in the emitter.        */
    int32 ActiveParticles;
    /** Monotonically increasing counter. */
    uint32 ParticleCounter;
    /** The maximum number of active particles that can be held in
     *  the particle data array.
     */
    int32 MaxActiveParticles;
    /** The fraction of time left over from spawning.                   */

    void SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity, struct FParticleEventInstancePayload* EventPayload)
    {
        for (int32 i = 0; i < Count; i++)
        {
            DECLARE_PARTICLE_PTR(Particle, ParticleData + (i * ParticleStride));
            //PreSpawn(Particle, InitialLocation, InitialVelocity);

            // for (int32 ModuleIndex = 0; ModuleIndex < LODLevel->SpawnModules.Num(); ModuleIndex++)
            // {
                //LODLevel->SpawnModules[ModuleIndex]->Spawn();
            // }

            //PostSpawn(Particle, Interp, SpawnTime);
        }
    }

    void KillParticle(int32 Index);

};