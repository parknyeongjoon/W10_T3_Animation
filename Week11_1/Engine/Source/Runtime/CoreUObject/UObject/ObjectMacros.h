#pragma once
#include "ObjectFactory.h"
#include "Class.h"
#include "UObjectHash.h"
#include "ThirdParty/sol/sol.hpp"
#include "ScriptStruct.h"

// 문자열화 매크로
#define INLINE_STRINGIFY(name) #name

// 공통 클래스 정의 부분 (복사, 이동, 등록자, 타입별 별칭)
#define __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
private: \
    TClass(const TClass&) = delete; \
    TClass& operator=(const TClass&) = delete; \
    TClass(TClass&&) = delete; \
    TClass& operator=(TClass&&) = delete; \
    inline static struct TClass##_StaticClassRegistrar_PRIVATE \
    { \
        TClass##_StaticClassRegistrar_PRIVATE() \
        { \
            UClass::GetClassMap().Add(#TClass, ThisClass::StaticClass()); \
            AddClassToChildListMap(ThisClass::StaticClass()); \
            UClassRegistry::Get().RegisterClass(ThisClass::StaticClass()); \
        } \
    } TClass##_StaticClassRegistrar_PRIVATE{}; \
public: \
    using Super = TSuperClass; \
    using ThisClass = TClass; \
private: \
    static TMap<FString, std::function<void(sol::usertype<TClass>)>>& GetBindFunctions() { \
        static TMap<FString, std::function<void(sol::usertype<TClass>)>> _binds; \
        return _binds; \
    } \
public: \
    using InheritTypes = SolTypeBinding::InheritList<TClass, TSuperClass>::type; \
    static sol::usertype<TClass> GetLuaUserType(sol::state& lua) { \
        static sol::usertype<TClass> usertype = lua.new_usertype<TClass>( \
            #TClass, \
            sol::base_classes, \
            SolTypeBinding::TypeListToBases<typename SolTypeBinding::InheritList<TClass, TSuperClass>::base_list>::Get() \
        ); \
        return usertype; \
    } \
    static void BindPropertiesToLua(sol::state& lua) { \
        sol::usertype<TClass> table = GetLuaUserType(lua); \
        for (const auto& [name, bind] : GetBindFunctions()) \
        { \
            bind(table); \
        } \
        SolTypeBinding::RegisterGetComponentByClass<TClass>(lua, #TClass); \
        lua.set_function(std::string("As") + #TClass, [](UObject* obj)->TClass* { \
            return dynamic_cast<TClass*>(obj); \
        }); \
    }

// RTTI를 위한 클래스 매크로
#define DECLARE_CLASS(TClass, TSuperClass) \
    __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
    static UClass* StaticClass() { \
        static UClass ClassInfo{ \
            #TClass, \
            static_cast<uint32>(sizeof(TClass)), \
            static_cast<uint32>(alignof(TClass)), \
            TSuperClass::StaticClass(), \
            []() -> UObject* { \
                void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(sizeof(TClass)); \
                ::new (RawMemory) TClass; \
                return static_cast<UObject*>(RawMemory); \
            } \
        }; \
        ClassInfo.Creator = [](UObject* InOuter) -> void* { \
            return FObjectFactory::ConstructObject<TClass>(InOuter); \
        }; \
        ClassInfo.BindPropertiesToLua = TClass::BindPropertiesToLua; \
        return &ClassInfo; \
    } 

// RTTI를 위한 추상 클래스 매크로
#define DECLARE_ABSTRACT_CLASS(TClass, TSuperClass) \
    __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
    static UClass* StaticClass() { \
        static UClass ClassInfo{ \
            #TClass, \
            static_cast<uint32>(sizeof(TClass)), \
            static_cast<uint32>(alignof(TClass)), \
            TSuperClass::StaticClass(), \
            []() -> UObject* { return nullptr; } \
        }; \
        ClassInfo.BindPropertiesToLua = TClass::BindPropertiesToLua; \
        return &ClassInfo; \
    }


// ---------- DECLARE_STRUCT 관련 매크로 ----------
#define DECLARE_COMMON_STRUCT_BODY(TStruct, TSuperStruct) \
private: \
    inline static struct Z_##TStruct##_StructRegistrar_PRIVATE \
    { \
        Z_##TStruct##_StructRegistrar_PRIVATE() \
        { \
            UScriptStruct::GetScriptStructMap().Add(FName(INLINE_STRINGIFY(TStruct)), TStruct::StaticStruct()); \
        } \
    } Z_##TStruct##_StructRegistrar_Instance_PRIVATE{}; \
public: \
    using Super = TSuperStruct; \
    using ThisClass = TStruct;

#define DECLARE_STRUCT_WITH_SUPER(TStruct, TSuperStruct) \
    DECLARE_COMMON_STRUCT_BODY(TStruct, TSuperStruct) \
    static UScriptStruct* StaticStruct() \
    { \
        static UScriptStruct StructInfo{ \
            INLINE_STRINGIFY(TStruct), \
            static_cast<uint32>(sizeof(TStruct)), \
            static_cast<uint32>(alignof(TStruct)), \
            TSuperStruct::StaticStruct() \
        }; \
        return &StructInfo; \
    }

#define DECLARE_STRUCT_NO_SUPER(TStruct) \
    DECLARE_COMMON_STRUCT_BODY(TStruct, TStruct) \
    static UScriptStruct* StaticStruct() \
    { \
        static UScriptStruct StructInfo{ \
            INLINE_STRINGIFY(TStruct), \
            static_cast<uint32>(sizeof(TStruct)), \
            static_cast<uint32>(alignof(TStruct)), \
            nullptr \
        }; \
        return &StructInfo; \
    }

#define GET_OVERLOADED_STRUCT_MACRO(_1, _2, MACRO, ...) MACRO

#define EXPAND_MACRO(x) x
#define DECLARE_STRUCT(...) \
    EXPAND_MACRO(GET_OVERLOADED_STRUCT_MACRO(__VA_ARGS__, DECLARE_STRUCT_WITH_SUPER, DECLARE_STRUCT_NO_SUPER)(__VA_ARGS__))



// ---------- UProperty 관련 매크로 ----------
#define GET_FIRST_ARG(First, ...) First
#define FIRST_ARG(...) GET_FIRST_ARG(__VA_ARGS__, )

#define UPROPERTY_WITH_FLAGS(InFlags, InType, InVarName, ...) \
    InType InVarName FIRST_ARG(__VA_ARGS__); \
    inline static struct InVarName##_PropRegistrar_PRIVATE \
    { \
        InVarName##_PropRegistrar_PRIVATE() \
        { \
            constexpr int64 Offset = offsetof(ThisClass, InVarName); \
            constexpr EPropertyFlags Flags = InFlags; \
            UStruct* StructPtr = GetStructHelper<ThisClass>(); \
            StructPtr->AddProperty( \
                PropertyFactory::Private::MakeProperty<InType, Flags>(StructPtr, #InVarName, Offset) \
            ); \
        } \
    } InVarName##_PropRegistrar_PRIVATE{};

#define UPROPERTY_DEFAULT(InType, InVarName, ...) \
    UPROPERTY_WITH_FLAGS(EPropertyFlags::PropertyNone, InType, InVarName, __VA_ARGS__)

#define GET_OVERLOADED_PROPERTY_MACRO(_1, _2, _3, _4, NAME, ...) NAME

/**
 * UClass에 Property를 등록합니다.
 * @param Type 선언할 타입
 * @param VarName 변수 이름
 * @param ... 기본값
 *
 * ----- Example Code -----
 *
 * UPROPERTY(int, Value)
 *
 * UPROPERTY(int, Value, = 10)
 *
 * UPROPERTY(EPropertyFlags::EditAnywhere, int, Value, = 10) // Flag를 지정하면 기본값은 필수
 */
#define UPROPERTY(...) \
    EXPAND_MACRO(GET_OVERLOADED_PROPERTY_MACRO(__VA_ARGS__, UPROPERTY_WITH_FLAGS, UPROPERTY_DEFAULT, UPROPERTY_DEFAULT)(__VA_ARGS__))

/*
예시
UFUNCTION(
bool, IsActive, () const;
)

UFUNCTION(
void, Activate, ();
)

UFUNCTION(
void, Deactivate, ();
)

UFUNCTION(
bool, TestFunc, () const,
{
  return true;
}
)

*/
#define UFUNCTION(Type, FuncName, Args, Stmt) \
    Type FuncName Args Stmt \
    inline static struct FuncName##_PropRegister \
    { \
        FuncName##_PropRegister() \
        { \
            GetBindFunctions().Add(#FuncName, [](sol::usertype<ThisClass> table) { \
                table[#FuncName] = &ThisClass::FuncName; \
            }); \
        } \
    } FuncName##_PropRegister_{};

// ----- 반드시 포함: ACTORCOMPONENT INFO -----
#define DECLARE_ACTORCOMPONENT_INFO(T) \
    struct T##FactoryRegister { \
        T##FactoryRegister() {\
            GetFactoryMap()[#T] = []() -> std::unique_ptr<FActorComponentInfo> {\
                return std::make_unique<T>(); \
            }; \
        } \
    }; \
    static inline T##FactoryRegister Global_##T##_FactoryRegister;