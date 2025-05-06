#include "SkeletalMeshActor.h"
#include "TestFBXLoader.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"

ASkeletalMeshActor::ASkeletalMeshActor()
{
    USkeletalMeshComponent* SkeletalMeshComp = AddComponent<USkeletalMeshComponent>(EComponentOrigin::Editor);
    
    USkeletalMesh* SkeletalMesh = TestFBXLoader::CreateSkeletalMesh(TEXT("FBX/Wizard.fbx"));
    SkeletalMeshComp->SetSkeletalMesh(SkeletalMesh);
}
