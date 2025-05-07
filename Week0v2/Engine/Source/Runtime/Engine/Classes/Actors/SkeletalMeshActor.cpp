#include "SkeletalMeshActor.h"
#include "TestFBXLoader.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"

ASkeletalMeshActor::ASkeletalMeshActor()
{
    USkeletalMeshComponent* SkeletalMeshComp = AddComponent<USkeletalMeshComponent>(EComponentOrigin::Editor);
    USkeletalMesh* SkeletalMesh = SkeletalMeshComp->LoadSkeletalMesh(TEXT("FBX/example.fbx"));
    
    
    for (auto bone : SkeletalMesh->GetRefSkeletal()->BoneTree)
    {
        std::cout << bone.BoneName << std::endl;
    }
}