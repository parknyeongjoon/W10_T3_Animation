#include "WaitObjects.h"

FWaitForSeconds::FWaitForSeconds(float InSeconds)
    : RemainingTime(InSeconds)
{
}

bool FWaitForSeconds::IsReady(float DeltaTime)
{
    RemainingTime -= DeltaTime;
    return RemainingTime <= 0.0f;
}

FWaitUntil::FWaitUntil(sol::function ConditionFunc)
    : Condition(std::move(ConditionFunc))
{
}

bool FWaitUntil::IsReady(float DeltaTime)
{
    if (!Condition.valid())
        return true;

    sol::protected_function_result Result = Condition();
    if (!Result.valid())
        return true;

    return Result.get<bool>();
}

FWaitForFrames::FWaitForFrames(int32_t InFrameCount)
    : RemainingFrames(InFrameCount)
{
}

bool FWaitForFrames::IsReady(float DeltaTime)
{
    if (RemainingFrames > 0)
    {
        --RemainingFrames;
    }
    return RemainingFrames <= 0;
}

FWaitWhile::FWaitWhile(sol::function ConditionFunc)
    : Condition(std::move(ConditionFunc))
{
}

bool FWaitWhile::IsReady(float DeltaTime)
{
    if (!Condition.valid())
        return false;

    sol::protected_function_result Result = Condition();
    if (!Result.valid())
        return false;

    return !Result.get<bool>();
}