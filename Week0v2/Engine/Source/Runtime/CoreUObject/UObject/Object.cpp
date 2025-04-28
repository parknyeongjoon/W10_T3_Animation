#include "Object.h"

#include "EditorEngine.h"
#include "UClass.h"
#include "UObjectHash.h"


UClass* UObject::StaticClass()
{
    static UClass ClassInfo{TEXT("UObject"), sizeof(UObject), alignof(UObject), nullptr};
    return &ClassInfo;
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

UWorld* UObject::GetWorld()
{
    return GEngine->GetWorld();
}

UWorld* UObject::GetPIEWorld()
{
    return GEngine->GetPIEWorld();
}

bool UObject::IsA(const UClass* SomeBase) const
{
    const UClass* ThisClass = GetClass();
    return ThisClass->IsChildOf(SomeBase);
}


