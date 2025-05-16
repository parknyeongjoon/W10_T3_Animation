#include "UClass.h"
#include <cassert>


UClass::UClass(const char* InClassName, uint32 InClassSize, uint32 InAlignment, UClass* InSuperClass)
    : ClassSize(InClassSize)
    , ClassAlignment(InAlignment)
    , SuperClass(InSuperClass)
{
    NamePrivate = InClassName;
}

bool UClass::IsChildOf(const UClass* SomeBase) const
{
    assert(this);
    if (!SomeBase) return false;

    // Super의 Super를 반복하면서 
    for (const UClass* TempClass = this; TempClass; TempClass=TempClass->GetSuperClass())
    {
        if (TempClass == SomeBase)
        {
            return true;
        }
    }
    return false;
}

UObject* UClass::CreateDefaultObject()
{
    if (!ClassDefaultObject)
    {
        // TODO: CDO 객체 만들기
        ClassDefaultObject = nullptr;
    }

    return ClassDefaultObject;
}
