#pragma once
#include <sol/sol.hpp>
#include <string> // 또는 엔진의 문자열 타입 (예: FString)
#include <unordered_map>
#include <memory> // std::unique_ptr (싱글톤용)
#include <mutex>  // std::once_flag (싱글톤용)

class FLuaManager
{
public:
    sol::state Lua;
    void Initialize();
    void Release();
};
