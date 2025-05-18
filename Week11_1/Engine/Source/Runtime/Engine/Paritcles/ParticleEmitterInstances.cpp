#include "ParticleEmitterInstances.h"

#include "UserInterface/Console.h"

// FORCEINLINE static void* FastParticleSmallBlockAlloc(size_t AllocSize)
// {
//     if (GEnableFastPools)
//     {
//         FScopeLock S(&GFastPoolsCriticalSection);
//
//         FFastPoolFreePool* Allocations = GFastPoolFreedAllocations.Find(AllocSize);
//         if ( Allocations  && Allocations->FreeAllocations.Num() )
//         {
//             void* Result = Allocations->FreeAllocations[0];
//             Allocations->FreeAllocations.RemoveAtSwap(0, EAllowShrinking::No);
//             Allocations->LastUsedTime = FPlatformTime::Seconds();
//             GFreePoolSizeBytes -= AllocSize;
//             return Result;
//         }
//     }
//
//     return FPlatformMemory::Malloc(AllocSize);
// }

// FORCEINLINE static void FastParticleSmallBlockFree(void *RawMemory, size_t AllocSize)
// {
//     if (GEnableFastPools)
//     {
//         FScopeLock S(&GFastPoolsCriticalSection);
//         FFastPoolFreePool& Allocations = GFastPoolFreedAllocations.FindOrAdd(AllocSize);
//         Allocations.FreeAllocations.Add(RawMemory);
//         GFreePoolSizeBytes += AllocSize;
//
//         if ( GFreePoolSizeBytes > GMaxFreePoolSizeBytes )
//         {
//             // free the oldest allocation
//             FFastPoolFreePool* OldestPool = nullptr;
//             int32 OldestPoolAllocSize  = 0;
//             for ( auto& PoolItr : GFastPoolFreedAllocations )
//             {
//                 FFastPoolFreePool& Pool = PoolItr.Value;
//                 if ( Pool.FreeAllocations.Num() )
//                 {
//                     if ( OldestPool == nullptr )
//                     {
//                         OldestPoolAllocSize = PoolItr.Key;
//                         OldestPool = &Pool;
//                     }
//                     else
//                     {
//                         if (OldestPool->LastUsedTime > Pool.LastUsedTime)
//                         {
//                             OldestPoolAllocSize = PoolItr.Key;
//                             OldestPool = &Pool;
//                         }
//                     }
//                 }
//             }
//             check( OldestPool );
//             check( OldestPoolAllocSize  != 0 );
//             void* OldAllocation = OldestPool->FreeAllocations[0];
//             OldestPool->FreeAllocations.RemoveAtSwap(0, EAllowShrinking::No);
//             GFreePoolSizeBytes -= OldestPoolAllocSize;
//             FMemory::Free(OldAllocation);
//         }
//         return;
//     }
//     FMemory::Free(RawMemory);
// }

void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
    if(InParticleDataNumBytes <= 0 || ParticleIndicesNumShorts < 0
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
        if(MemBlockSize <= 0)
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
