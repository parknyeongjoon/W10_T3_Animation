#pragma once
#include "NameTypes.h"
#include "Container/String.h"

class UWorld;
class UClass;
class FFunctionRegistry;

class UObject
{
private:
    UObject& operator=(const UObject&) = delete;
    UObject(UObject&&) = delete;
    UObject& operator=(UObject&&) = delete;

public:
    using Super = UObject;
    using ThisClass = UObject;
    UObject(const UObject& Other)
        : ClassPrivate(Other.ClassPrivate)
        , NamePrivate(Other.NamePrivate)
        , InternalIndex(Other.InternalIndex)
    {
    }
    static UClass* StaticClass();
    static FFunctionRegistry* FunctionRegistry();

    virtual UObject* Duplicate(UObject* InOuter);

    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter){}; // 하위 클래스에서 override
    virtual void PostDuplicate(){};
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

    UObject* GetOuter();
    virtual UWorld* GetWorld();

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

public:
    void* operator new(size_t size);

    void operator delete(void* ptr, size_t size);
public:
    // Serialize

};
