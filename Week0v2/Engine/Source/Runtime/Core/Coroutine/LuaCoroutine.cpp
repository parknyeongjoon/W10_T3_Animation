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
        else if (yielded.is<FWaitForFrames*>())
        {
            WaitObject = yielded.as<FWaitForFrames*>();
        }
        else if (yielded.is<FWaitWhile*>())
        {
            WaitObject = yielded.as<FWaitWhile*>();
        }
        else
        {
            std::cout << "[MoveNext] Unknown yield type, terminating coroutine." << std::endl;
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

void RegisterWaitHelpers(sol::state& lua)
{
    // 1. 원본 C++ 클래스 등록
    lua.new_usertype<FWaitForSeconds>("FWaitForSeconds",
        sol::constructors<FWaitForSeconds(float)>());
    lua.new_usertype<FWaitForFrames>("FWaitForFrames",
        sol::constructors<FWaitForFrames(int32_t)>());
    lua.new_usertype<FWaitUntil>("FWaitUntil",
        sol::constructors<FWaitUntil(sol::function)>());
    lua.new_usertype<FWaitWhile>("FWaitWhile",
        sol::constructors<FWaitWhile(sol::function)>());

    // 2. Lua용 간단한 함수 등록
    lua["WaitForSeconds"] = [](float seconds) { return new FWaitForSeconds(seconds); };
    lua["WaitForFrames"] = [](int32_t frames) { return new FWaitForFrames(frames); };
    lua["WaitUntil"] = [](sol::function condition) { return new FWaitUntil(condition); };
    lua["WaitWhile"] = [](sol::function condition) { return new FWaitWhile(condition); };
}