#pragma once
#include <concepts>

#include "EngineStatics.h"
#include "Object.h"
#include "UObjectArray.h"
#include "HAL/PlatformType.h"
#include "UserInterface/Console.h"

class FObjectFactory
{
public:
    template <typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObject()
    {
        uint32 id = UEngineStatics::GenUUID();
        FString Name = T::StaticClass()->GetName() + "_" + std::to_string(id);

        T* Obj = new T; // TODO: FPlatformMemory::Malloc으로 변경, placement new 사용시 Free방법 생각하기
        Obj->ClassPrivate = T::StaticClass();
        Obj->NamePrivate = Name;
        Obj->UUID = id;

        GUObjectArray.AddObject(Obj);

        UE_LOG(LogLevel::Display, "Created New Object : %s", *Name);
        return Obj;
    }
    template <typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObjectFrom(const T* Source)
    {
        uint32 id = UEngineStatics::GenUUID();
        FString Name = T::StaticClass()->GetName() + "_Copy_" + std::to_string(id);

        T* Obj = new T(*Source); // 복사 생성자 사용
        Obj->ClassPrivate = T::StaticClass();
        Obj->NamePrivate = Name;
        Obj->UUID = id;

        GUObjectArray.AddObject(Obj);

        UE_LOG(LogLevel::Display, "Cloned Object : %s", *Name);
        return Obj;
    }
};
