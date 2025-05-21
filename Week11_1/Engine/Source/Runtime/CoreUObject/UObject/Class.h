#pragma once
#include <concepts>
#include "Struct.h"
#include "Container/Map.h"
#include "NameTypes.h"
#include "ThirdParty/sol/sol.hpp"
#include "Property.h"

//struct FProperty;
/**
 * UObject의 RTTI를 가지고 있는 클래스
 */
class UClass : public UStruct
{
    using Super = UStruct;
    using ThisClass = UClass;
    using ClassConstructorType = UObject * (*)();

public:
    //UClass(const char* InClassName, uint32 InClassSize, uint32 InAlignment, UClass* InSuperClass);
    UClass(
        const char* InClassName,
        uint32 InClassSize,
        uint32 InAlignment,
        UClass* InSuperClass,
        ClassConstructorType InCTOR
    );
    virtual ~UClass() override;

    // 복사 & 이동 생성자 제거
    UClass(const UClass&) = delete;
    UClass& operator=(const UClass&) = delete;
    UClass(UClass&&) = delete;
    UClass& operator=(UClass&&) = delete;

    static TMap<FName, UClass*>& GetClassMap();
    static UClass* FindClass(const FName& ClassName);

public:
    template <typename T>
    T* CreateObject(UObject* InOuter)
    {
        if (!Creator)
        {
            return nullptr;
        }
        return static_cast<T*>(Creator(InOuter));
    }

    /** SomeBase의 자식 클래스인지 확인합니다. */
    bool IsChildOf(const UClass* SomeBase) const;

    template <typename T>
        requires std::derived_from<T, UObject>
    bool IsChildOf() const
    {
        return IsChildOf(T::StaticClass());
    }

    /**
     * 부모의 UClass를 가져옵니다.
     *
     * @note AActor::StaticClass()->GetSuperClass() == UObject::StaticClass()
     */
    FORCEINLINE UClass* GetSuperClass() const
    {
        return static_cast<UClass*>(SuperStruct);  // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    }

    UObject* GetDefaultObject() const;

    template <typename T>
        requires std::derived_from<T, UObject>
    T* GetDefaultObject() const;

    const TArray<FProperty*>& GetProperties() const { return Properties; }
protected:
    virtual UObject* CreateDefaultObject();

public:
    ClassConstructorType ClassCTOR;

private:
    UObject* ClassDefaultObject = nullptr;

public:
    using ObjectCreator = void* (*)(UObject*);
    ObjectCreator Creator = nullptr;

    /** Lua에 UPROPERTY를 Bind하는 함수.
     *  DECLARE_CLASS에서 초기화됨
     */
    std::function<void(sol::state&)> BindPropertiesToLua;
};

class UClassRegistry
{
public:
    static UClassRegistry& Get()
    {
        static UClassRegistry Instance;
        return Instance;
    };

    void RegisterClass(UClass* InClass)
    {
        Registry.Add(InClass->GetFName(), InClass);
    }

    UClass* FindClassByName(FName ClassName)
    {
        if (Registry.Contains(ClassName))
        {
            return Registry[ClassName];
        }
        return nullptr;
    }

    TMap<FName, UClass*> Registry;
};

template <typename T>
    requires std::derived_from<T, UObject>
T* UClass::GetDefaultObject() const
{
    UObject* Ret = GetDefaultObject();
    assert(Ret->IsA<T>());
    return static_cast<T*>(Ret);
}
