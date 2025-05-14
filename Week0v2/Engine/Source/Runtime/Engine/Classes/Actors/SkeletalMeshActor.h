#pragma once
#include "Animation/CustomAnimInstance/TestAnimInstance.h"
#include "GameFramework/Actor.h"

class USkeletalMeshComponent;

class ASkeletalMeshActor : public AActor
{
    DECLARE_CLASS(ASkeletalMeshActor, AActor)
    
public:
    ASkeletalMeshActor();
    ASkeletalMeshActor(const ASkeletalMeshActor& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;

    USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComp; }

private:
    USkeletalMeshComponent* SkeletalMeshComp;
};
