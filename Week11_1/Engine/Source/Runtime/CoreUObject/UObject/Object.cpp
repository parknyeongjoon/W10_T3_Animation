#include "Object.h"

#include "Engine/Engine.h"
#include "FunctionRegistry.h"
#include "UserInterface/Console.h"

extern UEngine* GEngine;

UClass* UObject::StaticClass()
{
    static UClass ClassInfo{
        "UObject",
        sizeof(UObject),
        alignof(UObject),
        nullptr,
        []() -> UObject*
        {
            void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(sizeof(UObject));
            ::new (RawMemory) UObject;
            return static_cast<UObject*>(RawMemory);
        }
    };
    return &ClassInfo;
}

FFunctionRegistry* UObject::FunctionRegistry()
{
    static FFunctionRegistry FunctionRegister;
    return &FunctionRegister;
}

UObject* UObject::Duplicate(UObject* InOuter)
{
    const auto NewObject = FObjectFactory::ConstructObject(GetClass(), InOuter);
    NewObject->DuplicateSubObjects(this, InOuter);       // 깊은 복사 수행
    return NewObject;
}

UObject::UObject()
    : UUID(0)
    // TODO: Object를 생성할 때 직접 설정하기
    , InternalIndex(std::numeric_limits<uint32>::max())
    , NamePrivate("None")
{
}

UObject::~UObject()
{
    NamePrivate = "Destroyed";
    UUID = 0;
}

UObject* UObject::GetOuter() const
{
    return OuterPrivate;
}

UWorld* UObject::GetWorld() const
{
    if (const UObject* Outer = GetOuter())
    {
        return Outer->GetWorld();
    }
    return nullptr;
}


bool UObject::IsA(const UClass* SomeBase) const
{
    const UClass* ThisClass = GetClass();
    return ThisClass->IsChildOf(SomeBase);
}

void* UObject::operator new(const size_t Size)
{
    UE_LOG(LogLevel::Display, "UObject Created : %d", Size);

    void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(Size);
    UE_LOG(
        LogLevel::Display,
        "TotalAllocationBytes : %d, TotalAllocationCount : %d",
        FPlatformMemory::GetAllocationBytes<EAT_Object>(),
        FPlatformMemory::GetAllocationCount<EAT_Object>()
    );
    return RawMemory;
}

void UObject::operator delete(void* Ptr, const size_t Size)
{
    UE_LOG(LogLevel::Display, "UObject Deleted : %d", Size);
    FPlatformMemory::Free<EAT_Object>(Ptr, Size);
}


void UObject::MarkAsGarbage()
{
    GUObjectArray.MarkRemoveObject(this);
}
