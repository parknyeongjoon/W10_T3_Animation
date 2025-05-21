#include "RandomStream.h"
#include <random>

// 귀찮으면 이거쓰기
FRandomStream* FRandomStream::GetDefaultStream()
{
    if (!DefaultRandomStream)
    {
        std::random_device rd;                           // 시드 생성기
        std::mt19937 gen(rd());                          // Mersenne Twister 엔진 (시드로 초기화)
        std::uniform_int_distribution<int32> dist(INT32_MIN, INT32_MAX);  // 균등 분포 [min, max]
        FRandomStream::DefaultRandomStream = new FRandomStream;
        FRandomStream::DefaultRandomStream->Initialize(dist(gen));
    }
    return DefaultRandomStream;
}
