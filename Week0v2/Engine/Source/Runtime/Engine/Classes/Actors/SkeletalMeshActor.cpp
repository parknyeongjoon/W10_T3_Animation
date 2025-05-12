#include "SkeletalMeshActor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/CustomAnimInstance/TestAnimInstance.h"
ASkeletalMeshActor::ASkeletalMeshActor()
{
    USkeletalMeshComponent* SkeletalMeshComp = AddComponent<USkeletalMeshComponent>(EComponentOrigin::Constructor);
    RootComponent = SkeletalMeshComp;
    SkeletalMeshComp->SetData("Contents/FBX/Rumba_Dancing.fbx");

    UTestAnimInstance* TestAnimInstance = FObjectFactory::ConstructObject<UTestAnimInstance>(this);
    SkeletalMeshComp->SetAnimInstance(TestAnimInstance);
}