#include "TestAnimInstance.h"
#include "Animation/AnimationStateMachine.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimNotify/AnimNotify.h"
UTestAnimInstance::UTestAnimInstance()
{
    IdleSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);
    WalkSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);
    DanceSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);

    TestFBXLoader::CreateSkeletalMesh("Contents/FBX/Idle.fbx");
    TestFBXLoader::CreateSkeletalMesh("Contents/FBX/Walking.fbx");
    TestFBXLoader::CreateSkeletalMesh("Contents/FBX/Rumba_Dancing.fbx");

    IdleSequence->SetData("Contents/FBX/Idle.fbx\\mixamo.com");
    WalkSequence->SetData("Contents/FBX/Walking.fbx\\mixamo.com");
    DanceSequence->SetData("Contents/FBX/Rumba_Dancing.fbx\\mixamo.com");

    AnimStateMachine = FObjectFactory::ConstructObject<UAnimationStateMachine<ETestState>>(this);

    //**State 추가**
    AnimStateMachine->AddState(ETestState::Idle, [this](float DeltaTime) {
        UpdateAnimation(IdleSequence, DeltaTime);
        });

    AnimStateMachine->AddState(ETestState::Walking, [this](float DeltaTime) {
        UpdateAnimation(WalkSequence, DeltaTime);
        });

    AnimStateMachine->AddState(ETestState::Dancing, [this](float DeltaTime) {
        UpdateAnimation(DanceSequence, DeltaTime);
        });

    // **Transition Rule 정의**
    AnimStateMachine->AddTransition(ETestState::Idle, ETestState::Walking, [&]() { 
        return (GetAsyncKeyState('Z') & 0x8000);
        });

    AnimStateMachine->AddTransition(ETestState::Walking, ETestState::Dancing, [&]() {
        return (GetAsyncKeyState('X') & 0x8000);
        });

    AnimStateMachine->AddTransition(ETestState::Dancing, ETestState::Idle, [&]() {
        return (GetAsyncKeyState('C') & 0x8000);
        });




    // 초기 상태 설정
    AnimStateMachine->SetState(ETestState::Idle);




}
UTestAnimInstance::~UTestAnimInstance()
{
}

UTestAnimInstance::UTestAnimInstance(const UTestAnimInstance& Other) : 
    UAnimInstance(Other),
    AnimStateMachine(Other.AnimStateMachine),
    IdleSequence(Other.IdleSequence),
    WalkSequence(Other.WalkSequence),
    DanceSequence(Other.DanceSequence)
{
}

UObject* UTestAnimInstance::Duplicate(UObject* InOuter)
{
    UTestAnimInstance* NewComp = FObjectFactory::ConstructObjectFrom<UTestAnimInstance>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}


void UTestAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    //USkeletalMeshComponent* SkeletalMesh = GetOwningComponent();
    //AActor* OwnerPawn = SkeletalMesh->GetOwner();

    if (AnimStateMachine) AnimStateMachine->Update(DeltaSeconds);

}
