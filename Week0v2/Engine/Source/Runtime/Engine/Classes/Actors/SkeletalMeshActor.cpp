#include "SkeletalMeshActor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/CustomAnimInstance/TestAnimInstance.h"
ASkeletalMeshActor::ASkeletalMeshActor()
{
    USkeletalMeshComponent* SkeletalMeshComp = AddComponent<USkeletalMeshComponent>(EComponentOrigin::Constructor);
    RootComponent = SkeletalMeshComp;
    SkeletalMeshComp->SetData("Contents/FBX/Rumba_Dancing.fbx");

    UTestAnimInstance* TestAnimInstance = FObjectFactory::ConstructObject<UTestAnimInstance>(SkeletalMeshComp);
    SkeletalMeshComp->SetAnimInstance(TestAnimInstance);
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
