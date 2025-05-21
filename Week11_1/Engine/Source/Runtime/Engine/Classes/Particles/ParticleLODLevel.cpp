#include "ParticleLODLevel.h"
#include "Particles/Modules/ParticleModule.h"


void UParticleLODLevel::AnalyzeModules()
{
    SpawnModules.Empty();
    UpdateModules.Empty();

    for (UParticleModule* Module : Modules)
    {
        if (!Module)
            continue;

        if (Module->IsSpawnModule())
        {
            SpawnModules.Add(Module);
        }

        if (Module->IsUpdateModule())
        {
            UpdateModules.Add(Module);
        }
    }
}
