#pragma once
#include "GameFramework/Actor.h"

class ASkeletalMeshActor : public AActor
{
    DECLARE_CLASS(ASkeletalMeshActor, AActor)
    
public:
    ASkeletalMeshActor();
    ASkeletalMeshActor(const ASkeletalMeshActor& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
};