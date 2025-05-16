#pragma once
#include <concepts>
#include "Object.h"
#include "Container/Map.h"
#include "NameTypes.h"
#include "ThirdParty/sol/sol.hpp"
#include "Property.h"

/**
 * UObject의 RTTI를 가지고 있는 클래스
 */
    class UClass : public UObject
{

    using ClassConstructorType = UObject * (*)();

public:
    UClass(const char* InClassName, uint32 InClassSize, uint32 InAlignment, UClass* InSuperClass);
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

    /** 컴파일 타임에 알 수 없는 프로퍼티 타입을 런타임에 검사합니다. */
    static void ResolvePendingProperties();

private:
    /** 컴파일 타임에 알 수 없는 프로퍼티 목록들 */
    static TArray<FProperty*>& GetUnresolvedProperties();

public:

    uint32 GetClassSize() const { return ClassSize; }
    uint32 GetClassAlignment() const { return ClassAlignment; }

    using ObjectCreator = void* (*)(UObject*);
    ObjectCreator Creator = nullptr;

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
    UClass* GetSuperClass() const
    {
        return SuperClass;
    }

    UObject* GetDefaultObject() const
    {
        if (!ClassDefaultObject)
        {
            const_cast<UClass*>(this)->CreateDefaultObject();
        }
        return ClassDefaultObject;
    }

    /** Lua에 UPROPERTY를 Bind하는 함수.
     *  DECLARE_CLASS에서 초기화됨
     */
    std::function<void(sol::state&)> BindPropertiesToLua;

    void RegisterProperty(FProperty* Prop);

    TArray<FProperty*> Properties;

protected:
    virtual UObject* CreateDefaultObject();

public:
    ClassConstructorType ClassCTOR;

private:
    [[maybe_unused]]
    uint32 ClassSize;

    [[maybe_unused]]
    uint32 ClassAlignment;

    UClass* SuperClass = nullptr;

    UObject* ClassDefaultObject = nullptr;
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