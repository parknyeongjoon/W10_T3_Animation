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
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* SourceObj) override;
    virtual void PostDuplicate() override;
    
    ULevel(const ULevel& Other);

private:
    TArray<AActor*> Actors;

public:
    TArray<AActor*>& GetActors() { return Actors; }
    TSet<AActor*> PendingBeginPlayActors;
    
    void PostLoad(); //씬컴포넌트 연결을 위해 모두 로드후 부착
};
