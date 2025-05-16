#include "LuaCoroutine.h"
#include "Delegates/DelegateCombination.h"
#include "WaitObjects.h"
#include "Launch/EditorEngine.h"
#include "UObject/Casts.h"

extern UEngine* GEngine;

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

    lua["Log"] = [](const std::string& message)
        {
            std::cout << "[LuaLog] " << message << std::endl;
        };
    lua.set_function("DeltaTime", []() {
        return FCoroutineManager::CurrentDeltaTime;
        });
}

void TestCoroutine(sol::state& lua)
{

    // Wait Helper 등록
    RegisterWaitHelpers(lua);

    // Lua 스크립트 로드
    if (lua.script_file("my_coroutine.lua").valid() == false)
    {
        std::cout << "Failed to load my_coroutine.lua" << std::endl;
        return;
    }

    // my_coroutine 함수 가져오기
    sol::function coroutineFunc = lua["my_coroutine"];
    if (!coroutineFunc.valid())
    {
        std::cout << "Failed to find my_coroutine function" << std::endl;
        return;
    }

    DECLARE_DELEGATE(TEST_DELEGATE, void);
    TEST_DELEGATE delegate;

    delegate.BindLambda([coroutineFunc] {
        sol::coroutine Co(coroutineFunc.lua_state(), coroutineFunc);
        FLuaCoroutine* NewCoroutine = new FLuaCoroutine(Co);
        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            EditorEngine->CoroutineManager.StartCoroutine(NewCoroutine);
        }
        });
    delegate.Execute();
    //Test Code.
}
