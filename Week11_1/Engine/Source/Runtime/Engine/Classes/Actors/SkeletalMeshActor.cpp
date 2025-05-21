#include "SkeletalMeshActor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/CustomAnimInstance/TestAnimInstance.h"
ASkeletalMeshActor::ASkeletalMeshActor()
{
    SkeletalMeshComp = AddComponent<USkeletalMeshComponent>(EComponentOrigin::Constructor);
    SkeletalMeshComp->SetData("Contents/FBX/Rumba_Dancing.fbx");
    UTestAnimInstance* TestAnimInstance = FObjectFactory::ConstructObject<UTestAnimInstance>(SkeletalMeshComp);
    SkeletalMeshComp->SetAnimInstance(TestAnimInstance);
    RootComponent = SkeletalMeshComp;
}


UObject* ASkeletalMeshActor::Duplicate(UObject* InOuter)
{
    ASkeletalMeshActor* ClonedActor = Cast<ThisClass>(Super::Duplicate(InOuter));
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void ASkeletalMeshActor::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    ASkeletalMeshActor* ClonedActor = Cast<ASkeletalMeshActor>(Source);
    SkeletalMeshComp = Cast<USkeletalMeshComponent>(ClonedActor->SkeletalMeshComp->Duplicate(this));
    AddDuplicatedComponent(SkeletalMeshComp);
}