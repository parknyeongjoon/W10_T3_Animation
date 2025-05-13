#include "SkeletalMeshActor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/CustomAnimInstance/TestAnimInstance.h"
ASkeletalMeshActor::ASkeletalMeshActor()
{

}

ASkeletalMeshActor::ASkeletalMeshActor(const ASkeletalMeshActor& Other)
    :AActor(Other)
{
}

UObject* ASkeletalMeshActor::Duplicate(UObject* InOuter)
{
    ASkeletalMeshActor* NewComp = FObjectFactory::ConstructObjectFrom<ASkeletalMeshActor>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}
