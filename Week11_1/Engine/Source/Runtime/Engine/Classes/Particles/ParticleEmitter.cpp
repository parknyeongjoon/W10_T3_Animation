#include "ParticleEmitter.h"
#include <UserInterface/Console.h>

UParticleLODLevel* UParticleEmitter::GetLODLevel(int level)
{
    if (level >= 0 && level < LODLevels.Num())
    {
        return LODLevels[level];
    }
    else {
        UE_LOG(LogLevel::Warning, "Invalid LodLevels Index");
        return nullptr;
    }
}
