#include "SkeletalMeshActor.h"
#include "TestFBXLoader.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"

ASkeletalMeshActor::ASkeletalMeshActor()
{
    USkeletalMeshComponent* SkeletalMeshComp = AddComponent<USkeletalMeshComponent>(EComponentOrigin::Editor);
    USkeletalMesh* SkeletalMesh = SkeletalMeshComp->LoadSkeletalMesh(TEXT("FBX/Zombie.fbx"));
    SkeletalMesh->StoreOriginalBoneData();
}