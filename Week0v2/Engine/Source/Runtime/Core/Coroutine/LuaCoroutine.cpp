#include "LuaCoroutine.h"
#include "WaitObjects.h"

FLuaCoroutine::FLuaCoroutine(sol::coroutine coroutine)
    : Co(std::move(coroutine))
{
}

FLuaCoroutine::~FLuaCoroutine()
{
    if (WaitObject)
    {
        delete WaitObject;
        WaitObject = nullptr;
    }
}

bool FLuaCoroutine::MoveNext(float DeltaTime)
{
    if (bMarkedForDeletion)
        return false;

    if (WaitObject)
    {
        if (!WaitObject->IsReady(DeltaTime))
        {
            return true;
        }
        delete WaitObject;
        WaitObject = nullptr;
    }

    sol::protected_function_result result = Co();

    if (result.status() == sol::call_status::yielded)
    {
        sol::object yielded = result;
        if (yielded.is<FWaitForSeconds*>())
        {
            WaitObject = yielded.as<FWaitForSeconds*>();
        }
        else if (yielded.is<FWaitUntil*>())
        {
            WaitObject = yielded.as<FWaitUntil*>();
        }
        else
        {
            bMarkedForDeletion = true;
        }
        return true;
    }

    bMarkedForDeletion = true;
    return false;
}

IWaitObject* FLuaCoroutine::GetCurrent() const
{
    return WaitObject;
}
