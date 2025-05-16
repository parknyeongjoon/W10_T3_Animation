#pragma once
#include "Container/Set.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Serialization/Archive.h"

class AActor;

class ULevel : public UObject
{
    DECLARE_CLASS(ULevel, UObject)
public:
    ULevel();
    ~ULevel();
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* SourceObj, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    
private:
    TArray<AActor*> Actors;
    /*
     * param1: Origin Object
     * param2: Duplicated Object
     */
    TMap<UObject*, UObject*> DuplicatedObjects;

public:
    TArray<AActor*>& GetActors() { return Actors; }
    TMap<UObject*, UObject*>& GetDuplicatedObjects() { return DuplicatedObjects; }
    TSet<AActor*> PendingBeginPlayActors;
    
    void PostLoad(); //씬컴포넌트 연결을 위해 모두 로드후 부착
};
