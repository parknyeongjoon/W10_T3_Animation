#include "PlatformMemory.h"

std::atomic<uint64> FPlatformMemory::ObjectAllocationBytes = 0;
std::atomic<uint64> FPlatformMemory::ObjectAllocationCount = 0;
std::atomic<uint64> FPlatformMemory::ContainerAllocationBytes = 0;
std::atomic<uint64> FPlatformMemory::ContainerAllocationCount = 0;
