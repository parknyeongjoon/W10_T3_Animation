#include "Class.h"
#include <cassert>
#include "EngineStatics.h"
#include "UObjectArray.h"

UClass::UClass(const char* InClassName, uint32 InClassSize, uint32 InAlignment, UClass * InSuperClass)
    : ClassSize(InClassSize)
    , ClassAlignment(InAlignment)
    , SuperClass(InSuperClass)
{
    NamePrivate = InClassName;
}

UClass::UClass(
    const char* InClassName,
    uint32 InClassSize,
    uint32 InAlignment,
    UClass* InSuperClass,
    ClassConstructorType InCTOR
)
    : ClassCTOR(InCTOR)
    , ClassSize(InClassSize)
    , ClassAlignment(InAlignment)
    , SuperClass(InSuperClass)
{
    NamePrivate = InClassName;
}

UClass::~UClass()
{
    for (const FProperty* Prop : Properties)
    {
        delete Prop;
    }
    Properties.Empty();

    delete ClassDefaultObject;
    ClassDefaultObject = nullptr;
}

TMap<FName, UClass*>& UClass::GetClassMap()
{
    static TMap<FName, UClass*> ClassMap;
    return ClassMap;
}

UClass* UClass::FindClass(const FName& ClassName)
{
    if (UClass** It = GetClassMap().Find(ClassName))
    {
        return *It;
    }
    return nullptr;
}

void UClass::ResolvePendingProperties()
{
    for (FProperty* Prop : GetUnresolvedProperties())
    {
        Prop->Resolve();
    }
    GetUnresolvedProperties().Empty();
}

TArray<FProperty*>& UClass::GetUnresolvedProperties()
{
    static TArray<FProperty*> UnresolvedProperties;
    return UnresolvedProperties;
}


bool UClass::IsChildOf(const UClass* SomeBase) const
{
    assert(this);
    if (!SomeBase) return false;

    // Super의 Super를 반복하면서 
    for (const UClass* TempClass = this; TempClass; TempClass = TempClass->GetSuperClass())
    {
        if (TempClass == SomeBase)
        {
            return true;
        }
    }
    return false;
}


UObject* UClass::GetDefaultObject() const
{
    if (!ClassDefaultObject)
    {
        const_cast<UClass*>(this)->CreateDefaultObject();
    }
    return ClassDefaultObject;
}


void UClass::RegisterProperty(FProperty* Prop)
{
    /* 왠지 모르겠지만 TArray(std::vector)를 사용하면 Debug모드에서 Iterator검사를 하게 되는데,
     * 이때 검사할 때 잘못된 Iterator를 역참조해서 프로그램이 터짐, 근데 또 Release에서는 검사를 안하니 잘 동작함.
     *
     * 이유를 찾아보니 static 변수 초기화 순서 문제일 가능성이 있음.
     * 컴파일러마다 다를 수 있지만, UnresolvedProperties가 다른 static 변수보다 늦게 초기화가 되면,
     * 이터레이터 추적을 위한 내부 프록시 객체(_Mypair._Myval2._Myproxy)가 제대로 설정되지 않은 상태일 수 있기 때문에 문제가 생길 수 있음
     * TQueue도 마찬가지, TQueue는 Iterator검사는 안하지만, UnresolvedProperties가 가장 늦게 초기화 되는경우, 기존에 데이터가 없어지는 문제가 생겼음
     */
    switch (Prop->Type)  // NOLINT(clang-diagnostic-switch-enum)
    {
    case EPropertyType::UnresolvedPointer:
    {
        GetUnresolvedProperties().Add(Prop);
        break;
    }
    default:
        break;
    }

    Properties.Add(Prop);
}


UObject* UClass::CreateDefaultObject()
{
    if (!ClassDefaultObject)
    {
        ClassDefaultObject = ClassCTOR();
        if (!ClassDefaultObject)
        {
            return nullptr;
        }

        ClassDefaultObject->ClassPrivate = this;
        ClassDefaultObject->NamePrivate = GetName() + "_CDO";
        ClassDefaultObject->UUID = UEngineStatics::GenUUID();
        GUObjectArray.AddObject(ClassDefaultObject);
    }
    return ClassDefaultObject;
}