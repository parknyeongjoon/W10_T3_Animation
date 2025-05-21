#pragma once

#include "JungleMath.h"
#include "Math/Vector.h"

// 난수 생성기
struct FRandomStream
{
    FRandomStream()
        : InitialSeed(0)
        , Seed(0)
    {
    }

    void Initialize(int32 InSeed)
    {
        InitialSeed = InSeed;
        Seed = uint32(InSeed);
    }

    void Reset()
    {
        Seed = uint32(InitialSeed);
    }

    int32 GetInitialSeed()
    {
        return InitialSeed;
    }

    // [0,1)에서의 무작위 float를 반환합니다.
    float GetFraction() const
    {
        MutateSeed();

        float Result;

        *(uint32*)&Result = 0x3F800000U | (Seed >> 9);

        return Result - 1.0f;
    }

    // 0에서 Max까지의 무작위 int를 반환합니다.
    uint32 GetUnsignedInt() const
    {
        MutateSeed();

        return Seed;
    }

    // Unit Size의 무작위 Vector를 반환합니다.
    FVector GetUnitVector() const
    {
        FVector Result;
        float L;

        do
        {
            // Check random vectors in the unit sphere so result is statistically uniform.
            Result.X = GetFraction() * 2.f - 1.f;
            Result.Y = GetFraction() * 2.f - 1.f;
            Result.Z = GetFraction() * 2.f - 1.f;
            L = Result.MagnitudeSquared();
        } while (L > 1.f || L < KINDA_SMALL_NUMBER);

        return Result.GetSafeNormal();
    }

    int32 GetCurrentSeed() const
    {
        return int32(Seed);
    }

    // 무작위 float를 반환합니다.
    FORCEINLINE float FRand() const
    {
        return GetFraction();
    }

    // [0,A) 범위의 무작위 int를 반환합니다.
    FORCEINLINE int32 RandHelper(int32 A) const
    {
        // GetFraction guarantees a result in the [0,1) range.
        return ((A > 0) ? static_cast<int32>(GetFraction() * float(A)) : 0);
    }

    // [Min, Max] 범위의 무작위 int를 반환합니다.
    FORCEINLINE int32 RandRange(int32 Min, int32 Max) const
    {
        const int32 Range = (Max - Min) + 1;

        return Min + RandHelper(Range);
    }

    // [Min, Max] 범위의 무작위 float를 반환합니다.
    FORCEINLINE float FRandRange(float Min, float Max) const
    {
        return Min + (Max - Min) * GetFraction();
    }

    // 귀찮으면 이거쓰기
    static FRandomStream* GetDefaultStream();

protected:
    // 다음 seed로 넘어감
    void MutateSeed() const
    {
        Seed = (Seed * 196314165U) + 907633515U;
    }

    int32 InitialSeed;
    mutable uint32 Seed;

private:
    inline static FRandomStream* DefaultRandomStream = nullptr;
};

