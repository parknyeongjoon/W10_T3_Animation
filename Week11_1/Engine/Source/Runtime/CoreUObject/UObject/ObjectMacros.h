// ReSharper disable CppClangTidyBugproneMacroParentheses
// ReSharper disable CppClangTidyClangDiagnosticPedantic
#pragma once
#include "Class.h"
#include "UObjectHash.h"
#include "Templates/TypeUtilities.h"

// name을 문자열화 해주는 매크로
#define INLINE_STRINGIFY(name) #name


// 공통 클래스 정의 부분
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
        ClassInfo.Creator = [](UObject* InOuter) -> void* { return FObjectFactory::ConstructObject<TClass>(InOuter); }; \
        ClassInfo.BindPropertiesToLua = TClass::BindPropertiesToLua; \
        }; \
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
        return &ClassInfo; \
    }

#define DECLARE_ACTORCOMPONENT_INFO(T) \
    struct T##FactoryRegister { \
        T##FactoryRegister() {\
            GetFactoryMap()[#T] = []() -> std::unique_ptr<FActorComponentInfo> {\
                return std::make_unique<T>(); \
            }; \
        } \
    }; \
    static inline T##FactoryRegister Global_##T##_FactoryRegister; \


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
        } \
    } InVarName##_PropRegistrar_PRIVATE{};

#define UPROPERTY_DEFAULT(InType, InVarName, ...) \
    UPROPERTY_WITH_FLAGS(EPropertyFlags::PropertyNone, InType, InVarName, __VA_ARGS__)n

#define EXPAND_PROPERTY_MACRO(x) x
#define GET_OVERLOADED_PROPERTY_MACRO(_1, _2, _3, _4, NAME, ...) NAME


#define DECLARE_ACTORCOMPONENT_INFO(T) \
    struct T##FactoryRegister { \
        T##FactoryRegister() {\
            GetFactoryMap()[#T] = []() -> std::unique_ptr<FActorComponentInfo> {\
                return std::make_unique<T>(); \
            }; \
        } \
    }; \
    static inline T##FactoryRegister Global_##T##_FactoryRegister; \


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
    } FuncName##_PropRegister_{}; \

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
    } FuncName##_PropRegister_{}; \


