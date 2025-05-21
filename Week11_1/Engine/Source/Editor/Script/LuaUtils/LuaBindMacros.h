#pragma once
#include "LuaBindUtils.h"

/**
 * Lua에 새로운 UserType을 추가합니다.
 * @param TypeName lua에서 사용될 커스텀 타입
 * @note Ex) sol::state Lua; Lua.Lua_New_UserType(FTestStruct, ...);
 */
#define Lua_NewUserType(TypeName, ...) new_usertype<TypeName>(#TypeName, __VA_ARGS__)


#define LUA_BIND_CONSTRUCTORS(...) \
sol::call_constructor, \
sol::constructors<__VA_ARGS__>()

#define LUA_BIND_MEMBER(MemberName) \
LuaBindUtils::GetMemberName(#MemberName, true), MemberName

#define LUA_BIND_FUNC(MemberName) LUA_BIND_MEMBER(MemberName)

#define LUA_BIND_VAR(MemberName) \
LuaBindUtils::GetMemberName(#MemberName), sol::var(MemberName)