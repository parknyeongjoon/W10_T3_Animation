#pragma once

#include "IWaitObject.h"
#include <sol/sol.hpp>

class FWaitForSeconds : public IWaitObject
{
public:
    explicit FWaitForSeconds(float InSeconds);
    virtual bool IsReady(float DeltaTime) override;

private:
    float RemainingTime;
};

class FWaitUntil : public IWaitObject
{
public:
    explicit FWaitUntil(sol::function ConditionFunc);
    virtual bool IsReady(float DeltaTime) override;

private:
    sol::function Condition;
};
