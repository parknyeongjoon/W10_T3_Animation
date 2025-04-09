#include "UObjectArray.h"
#include "Object.h"
#include "UObjectHash.h"


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
        delete Object;
    }
    PendingDestroyObjects.Empty();
}

FUObjectArray GUObjectArray;
