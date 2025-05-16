#pragma once
#include "ObjectFactory.h"
#include "Class.h"
#include "UObjectHash.h"
#include "Templates/TypeUtilities.h"
#include "ThirdParty/sol/sol.hpp"

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
            UClassRegistry::Get().RegisterClass(ThisClass::StaticClass()); \
        } \
    } TClass##_StaticClassRegistrar_PRIVATE{}; \
public: \
    using Super = TSuperClass; \
    using ThisClass = TClass;

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
                void* RawMemory = FPlatformMemory::AlignedMalloc<EAT_Object>(sizeof(TClass), alignof(TClass)); \
                ::new (RawMemory) TClass; \
                return static_cast<UObject*>(RawMemory); \
            } \
        }; \
        ClassInfo.Creator = [](UObject* InOuter) -> void* { \
            return FObjectFactory::ConstructObject<TClass>(InOuter); \
        }; \
        ClassInfo.BindPropertiesToLua = TClass::BindPropertiesToLua; \
        return &ClassInfo; \
    } \
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
        return &ClassInfo; \
    }

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
            ThisClass::StaticClass()->RegisterProperty( \
                PropertyFactory::Private::MakeProperty<InType, Flags>(ThisClass::StaticClass(), #InVarName, Offset) \
            ); \
            GetBindFunctions().Add(#InVarName, [](sol::usertype<ThisClass> table) { \
                table[#InVarName] = &ThisClass::InVarName; \
            }); \
        } \
    } InVarName##_PropRegistrar_PRIVATE{};

#define UPROPERTY_DEFAULT(InType, InVarName, ...) \
    UPROPERTY_WITH_FLAGS(EPropertyFlags::PropertyNone, InType, InVarName, __VA_ARGS__)

#define EXPAND_PROPERTY_MACRO(x) x
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
    EXPAND_PROPERTY_MACRO(GET_OVERLOADED_PROPERTY_MACRO(__VA_ARGS__, UPROPERTY_WITH_FLAGS, UPROPERTY_DEFAULT, UPROPERTY_DEFAULT)(__VA_ARGS__))

 // Getter & Setter 생성
#define PROPERTY(type, name) \
private: \
    type name; \
public: \
    void Set##name(const type& value) { name = value; } \
    type Get##name() const { return name; }

// UFUNCTION 자동 Lua 바인딩 + 선언
#define UFUNCTION(Type, FuncName, ...) \
    Type FuncName (__VA_ARGS__); \
    inline static struct FuncName##_PropRegister \
    { \
        FuncName##_PropRegister() \
        { \
            GetBindFunctions().Add(#FuncName, [](sol::usertype<ThisClass> table) { \
                table[#FuncName] = &ThisClass::FuncName; \
            }); \
        } \
    } FuncName##_PropRegister_{};

#define UFUNCTION_CONST(Type, FuncName, ...) \
    Type FuncName (__VA_ARGS__) const; \
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