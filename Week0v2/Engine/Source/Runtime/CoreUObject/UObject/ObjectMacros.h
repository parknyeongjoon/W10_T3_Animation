// ReSharper disable CppClangTidyBugproneMacroParentheses
#pragma once
#include "UClass.h"
#include "ObjectFactory.h"
//#include "GameFramework/Actor.h"

// name을 문자열화 해주는 매크로
#define INLINE_STRINGIFY(name) #name


// RTTI를 위한 클래스 매크로
#define DECLARE_CLASS(TClass, TSuperClass) \
private: \
    TClass& operator=(const TClass&) = delete; \
    TClass(TClass&&) = delete; \
    TClass& operator=(TClass&&) = delete; \
public: \
    using Super = TSuperClass; \
    using ThisClass = TClass; \
    static UClass* StaticClass() { \
        static UClass ClassInfo{ TEXT(#TClass), static_cast<uint32>(sizeof(TClass)), static_cast<uint32>(alignof(TClass)), TSuperClass::StaticClass() }; \
        ClassInfo.Creator = [](UObject* InOuter) -> void* { return FObjectFactory::ConstructObject<TClass>(InOuter); }; \
        return &ClassInfo; \
    } \
private: \
    struct FAutoRegister_##TClass { \
        FAutoRegister_##TClass() { \
            UClassRegistry::Get().RegisterClass(TClass::StaticClass()); \
        } \
    }; \
public: \
    static inline FAutoRegister_##TClass AutoRegister_##TClass_Instance; \
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
        for (const auto [name, bind] : GetBindFunctions()) \
        { \
            bind(table); \
        } \
        SolTypeBinding::RegisterGetComponentByClass<TClass>(lua, #TClass); \
        lua.set_function(std::string("As") + #TClass, [](UObject* obj)->TClass* { \
            return dynamic_cast<TClass*>(obj); \
        }); \
    } \



#define DECLARE_ACTORCOMPONENT_INFO(T) \
    struct T##FactoryRegister { \
        T##FactoryRegister() {\
            GetFactoryMap()[#T] = []() -> std::unique_ptr<FActorComponentInfo> {\
                return std::make_unique<T>(); \
            }; \
        } \
    }; \
    static inline T##FactoryRegister Global_##T##_FactoryRegister; \




// #define PROPERTY(Type, VarName, DefaultValue) \
// private: \
//     Type VarName DefaultValue; \
// public: \
//     Type Get##VarName() const { return VarName; } \
//     void Set##VarName(const Type& New##VarName) { VarName = New##VarName; }

// Getter & Setter 생성
#define PROPERTY(type, name) \
    void Set##name(const type& value) { name = value; } \
    type Get##name() const { return name; }