#include "Object.h"

#include "Engine/Engine.h"
#include "FunctionRegistry.h"
#include "UserInterface/Console.h"

extern UEngine* GEngine;

UClass* UObject::StaticClass()
{
    static UClass ClassInfo{TEXT("UObject"), sizeof(UObject), alignof(UObject), nullptr};
    return &ClassInfo;
}

FFunctionRegistry* UObject::FunctionRegistry()
{
    static FFunctionRegistry FunctionRegister;
    return &FunctionRegister;
}

UObject* UObject::Duplicate(UObject* InOuter)
{
    UObject* NewObject = new UObject();
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
    if (UObject* Outer = GetOuter())
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

void* UObject::operator new(size_t size)
{
    UE_LOG(LogLevel::Display, "UObject Created : %d", size);

    void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(size);
    UE_LOG(
        LogLevel::Display,
        "TotalAllocationBytes : %d, TotalAllocationCount : %d",
        FPlatformMemory::GetAllocationBytes<EAT_Object>(),
        FPlatformMemory::GetAllocationCount<EAT_Object>()
    );
    return RawMemory;
}

void UObject::operator delete(void* ptr, size_t size)
{
    UE_LOG(LogLevel::Display, "UObject Deleted : %d", size);
    FPlatformMemory::Free<EAT_Object>(ptr, size);
}

// FVector4 UObject::EncodeUUID() const {
//     FVector4 result;
//
//     result.X = UUID & 0xFF;
//     result.Y = (UUID >> 8) & 0xFF;
//     result.Z = (UUID >> 16) & 0xFF;
//     result.W = (UUID >> 24) & 0xFF;
//
//     return result;
// }
