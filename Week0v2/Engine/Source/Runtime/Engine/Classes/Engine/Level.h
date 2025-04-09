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
    ULevel(const ULevel& Other)
    : UObject(Other)  // UObject 기반 클래스 복사
    , Actors(Other.Actors)  // TSet 얕은 복사
    , PendingBeginPlayActors(Other.PendingBeginPlayActors) // TArray 깊은 복사
{
    // 필요 시, 추가적인 복사 로직 구현 가능
        Actors.Empty();
}

private:
    TArray<AActor*> Actors;

public:
    TArray<AActor*>& GetActors() { return Actors; }
    TArray<AActor*> PendingBeginPlayActors;
};
