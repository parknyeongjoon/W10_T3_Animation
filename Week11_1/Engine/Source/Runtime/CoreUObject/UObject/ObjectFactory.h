#pragma once
#include "EngineStatics.h"
#include "Object.h"
#include "Class.h"
#include "UObjectArray.h"
#include "UserInterface/Console.h"

class FObjectFactory
{
public:
    static UObject* ConstructObject(UClass* InClass, UObject* InOuter, FName InName = NAME_None)
    {
        const uint32 Id = UEngineStatics::GenUUID();
        FName Name = FString::Printf(TEXT("%s_%d"), *InClass->GetName(), Id);

        if (InName != NAME_None)
        {
            Name = InName;
        }

        UObject* Obj = InClass->ClassCTOR();
        Obj->UUID = Id;
        Obj->NamePrivate = Name;
        Obj->ClassPrivate = InClass;
        Obj->OuterPrivate = InOuter;

        GUObjectArray.AddObject(Obj);

        UE_LOG(LogLevel::Display, "Created Object: %s, Size: %d", *Obj->GetName(), InClass->GetClassSize());

        return Obj;
    }
    template <typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObjectFrom(const T* Source, UObject* InOuter)
    {
        uint32 id = UEngineStatics::GenUUID();
        FString Name = T::StaticClass()->GetName() + "_Copy_" + std::to_string(id);

        T* Obj = new T(*Source); // 복사 생성자 사용
        Obj->ClassPrivate = T::StaticClass();
        Obj->NamePrivate = Name;
        Obj->OuterPrivate = InOuter;
        Obj->UUID = id;

        GUObjectArray.AddObject(Obj);

        UE_LOG(LogLevel::Display, "Cloned Object : %s", *Name);
        return Obj;
    }

    template<typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObject(UObject* InOuter)
    {
        return static_cast<T*>(ConstructObject(T::StaticClass(), InOuter));
    }

    template<typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObject(UObject* InOuter, FName InName)
    {
        return static_cast<T*>(ConstructObject(T::StaticClass(), InOuter, InName));
    }
};
