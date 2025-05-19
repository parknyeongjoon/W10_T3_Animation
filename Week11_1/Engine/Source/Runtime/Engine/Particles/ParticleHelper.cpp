#include "ParticleHelper.h"
#include "UserInterface/Console.h"

void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
    if (InParticleDataNumBytes <= 0 || ParticleIndicesNumShorts < 0
        || InParticleDataNumBytes % sizeof(uint16) != 0)
    {
        UE_LOG(LogLevel::Error, "ParticleDataContainer Alloc Failed");
        return;
    }// we assume that the particle storage has reasonable alignment below
    ParticleDataNumBytes = InParticleDataNumBytes;
    ParticleIndicesNumShorts = InParticleIndicesNumShorts;

    MemBlockSize = ParticleDataNumBytes + ParticleIndicesNumShorts * sizeof(uint16);

    //(uint8*)FastParticleSmallBlockAlloc(MemBlockSize); //임시방편으로 아래로 대체
    ParticleData = new uint8[MemBlockSize];

    ParticleIndices = (uint16*)(ParticleData + ParticleDataNumBytes);
}

void FParticleDataContainer::Free()
{
    if (ParticleData)
    {
        if (MemBlockSize <= 0)
        {
            UE_LOG(LogLevel::Error, "ParticleDataContainer Free Failed");
            return;
        }
        // FastParticleSmallBlockFree(ParticleData, MemBlockSize); //임시방편으로 아래로 대체
        delete[] ParticleData;
    }
    MemBlockSize = 0;
    ParticleDataNumBytes = 0;
    ParticleIndicesNumShorts = 0;
    ParticleData = nullptr;
    ParticleIndices = nullptr;
}
