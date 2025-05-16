#pragma once
#include "NameTypes.h"
#include "Container/String.h"
#include "sol/sol.hpp"

class UWorld;
class UClass;
class AActor;
class UActorComponent;
class FFunctionRegistry;

// for sol2 typing
namespace SolTypeBinding
{
    template<typename... Types>
    struct TypeList {};

    // PushBack
    template<typename List, typename NewType>
    struct PushBack;

    // Pushback
    template<typename... Types, typename NewType>
    struct PushBack<TypeList<Types...>, NewType> {
        using type = TypeList<Types..., NewType>;
    };

    // Base 클래스를 상속하는 모든 타입을 리스트로 모은다
    template<typename Derived, typename Base = void>
    struct InheritList;

    // Base 없는 경우 (Root 클래스)
    template<typename Derived>
    struct InheritList<Derived, void> {
        using type = TypeList<Derived>;
    };

    // Base 있는 경우 (Derived -> Base)
    template<typename Derived, typename Base>
    struct InheritList {
        using base_list = typename Base::InheritTypes;
        using type = typename PushBack<base_list, Derived>::type;
    };

    // to unpack types
    template<typename TypeList>
    struct TypeListToBases;

    // to unpack types
    template<typename... Types>
    struct TypeListToBases<TypeList<Types...>> {
        static auto Get() {
            return sol::bases<Types...>();
        }
    };

    // for Register to AActor::GetComponentByClass
    template <typename T>
    constexpr bool IsCompleteType_v = requires { sizeof(T); };

    // Register to AActor::GetComponentByClass
    template <typename T>
    void RegisterGetComponentByClass(sol::state& lua, std::string className)
    {
        if constexpr (IsCompleteType_v<AActor> && IsCompleteType_v<UActorComponent> && std::derived_from<T, UActorComponent>)
        {
            // 암시적 형변환에서 AActor가 완전한 타입임을 요구해서 명시적으로 형변환.
            using FuncType = T * (AActor::*)();
            auto funcPtr = static_cast<FuncType>(&AActor::template GetComponentByClass<T>);
            AActor::GetLuaUserType(lua)["Get" + className] = funcPtr;
            std::cout << "Register AActor::Get" << className << std::endl;
        }
        else
        {
            std::cout << "Failed Register AActor::Get" << className << std::endl;
        }
    }
}

class UObject
{
private:
    UObject& operator=(const UObject&) = delete;
    UObject(UObject&&) = delete;
    UObject& operator=(UObject&&) = delete;

public:
    using InheritTypes = SolTypeBinding::InheritList<UObject>::type;
    using Super = UObject;
    using ThisClass = UObject;
    UObject(const UObject& Other)
        : UUID(0)
        , InternalIndex(Other.InternalIndex)
        , NamePrivate(Other.NamePrivate)
        , ClassPrivate(Other.ClassPrivate)
    {
    }

    static UClass* StaticClass();
    static FFunctionRegistry* FunctionRegistry();

    virtual UObject* Duplicate(UObject* InOuter);

    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) {} // 하위 클래스에서 override
    virtual void PostDuplicate() {};
private:
    friend class FObjectFactory;
    friend class FSceneMgr;
    friend class UClass;

    uint32 UUID;
    uint32 InternalIndex; // Index of GUObjectArray

    FName NamePrivate;
    UClass* ClassPrivate = nullptr;
    UObject* OuterPrivate = nullptr;

public:
    UObject();
    virtual ~UObject();

    UObject* GetOuter() const;
    virtual UWorld* GetWorld() const;

    FName GetFName() const { return NamePrivate; }
    FString GetName() const { return NamePrivate.ToString(); }

    //TODO 이름 바꾸면 FNAME을 키값으로 하는 자료구조 모두 반영해야됨
    void SetFName(const FName& Name)
    {
        NamePrivate = Name;
    }

    uint32 GetUUID() const { return UUID; }
    uint32 GetInternalIndex() const { return InternalIndex; }

    UClass* GetClass() const { return ClassPrivate; }


    /** this가 SomeBase인지, SomeBase의 자식 클래스인지 확인합니다. */
    bool IsA(const UClass* SomeBase) const;

    template <typename T>
        requires std::derived_from<T, UObject>
    bool IsA() const
    {
        return IsA(T::StaticClass());
    }
    void MarkAsGarbage();

public:
    void* operator new(size_t Size);

    void operator delete(void* Ptr, size_t Size);
public:
    // Serialize

};
