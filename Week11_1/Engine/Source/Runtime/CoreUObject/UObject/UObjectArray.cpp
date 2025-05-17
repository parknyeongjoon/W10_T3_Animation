#include "UObjectArray.h"

#include "Class.h"
#include "Object.h"
#include "UObjectHash.h"
#include "Engine/UserInterface/Console.h"


void FUObjectArray::AddObject(UObject* Object)
{
    ObjObjects.Add(Object);
    AddToClassMap(Object);
}

void FUObjectArray::MarkRemoveObject(UObject* Object)
{
    ObjObjects.Remove(Object);
    RemoveFromClassMap(Object);  // UObjectHashTable에서 Object를 제외
    PendingDestroyObjects.AddUnique(Object);
}

void FUObjectArray::ProcessPendingDestroyObjects()
{
    for (UObject* Object : PendingDestroyObjects)
    {
        const UClass* Class = Object->GetClass();
        std::string ObjectName = *Object->GetName();
        const uint32 ObjectSize = Class->GetStructSize();

        std::destroy_at(Object);
        FPlatformMemory::Free<EAT_Object>(Object, ObjectSize);

        UE_LOG(LogLevel::Display, "Deleted Object: %s, Size: %d", ObjectName.c_str(), ObjectSize);
    }
    PendingDestroyObjects.Empty();
}

FUObjectArray GUObjectArray;
