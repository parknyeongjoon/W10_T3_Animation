#pragma once

#include "IEnumerator.h"
#include <sol/sol.hpp>

class FLuaCoroutine : public IEnumerator
{
public:
    explicit FLuaCoroutine(sol::coroutine coroutine);
    virtual ~FLuaCoroutine();

    virtual bool MoveNext(float DeltaTime) override;
    virtual IWaitObject* GetCurrent() const override;

private:
    sol::coroutine Co;
    IWaitObject* WaitObject = nullptr;
    bool bMarkedForDeletion = false;
};

void RegisterWaitHelpers(sol::state& lua);
void TestCoroutine(sol::state& lua);