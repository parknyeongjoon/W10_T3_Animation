#pragma once
#include "HAL/PlatformType.h"


/**
 * Windows 플랫폼에서의 시간 관련 기능을 제공하는 클래스
 */
class FWindowsPlatformTime
{
public:
    static double GSecondsPerCycle; // 0
    static bool bInitialized;       // false

    static void InitTiming();

    /**
     * CPU 사이클 당 초 수를 반환하는 함수
     * @return float 사이클 당 초 수
     */
    static float GetSecondsPerCycle();

    /**
     * CPU 주파수를 반환하는 함수
     * @return uint64 CPU 주파수
     */
    static uint64 GetFrequency();

    /**
     * 주어진 사이클 차이를 밀리초로 변환하는 함수
     * @param CycleDiff 사이클 차이
     * @return double 밀리초 단위의 시간
     */
    static double ToMilliseconds(uint64 CycleDiff);

    /**
     * 현재 CPU 사이클 수를 반환하는 함수
     * @return uint64 현재 CPU 사이클 수
     */
    static uint64 Cycles64();
};

typedef FWindowsPlatformTime FPlatformTime;
