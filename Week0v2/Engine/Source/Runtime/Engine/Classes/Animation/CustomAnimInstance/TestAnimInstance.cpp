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

    TestFBXLoader::CreateSkeletalMesh("Contents/FBX/Ninja_Idle.fbx");
    TestFBXLoader::CreateSkeletalMesh("Contents/FBX/Sneak_Walk.fbx");
    TestFBXLoader::CreateSkeletalMesh("Contents/FBX/Rumba_Dancing.fbx");

    IdleSequence->SetData("Contents/FBX/Ninja_Idle.fbx\\mixamo.com");
    WalkSequence->SetData("Contents/FBX/Sneak_Walk.fbx\\mixamo.com");
    DanceSequence->SetData("Contents/FBX/Rumba_Dancing.fbx\\mixamo.com");

    AnimStateMachine = FObjectFactory::ConstructObject<UAnimationStateMachine<ETestState>>(this);

    IdleCallback.func = [](UTestAnimInstance* self, float DeltaTime)
    {
        if (self->CurrentSequence != self->IdleSequence) {
            self->bIsBlending = true;
            self->BlendTime = 0.0f;
            self->PreviousSequence = self->CurrentSequence;
            self->CurrentSequence = self->IdleSequence;
        }
        if (self->bIsBlending) {
            self->BlendAnimations(self->PreviousSequence, self->CurrentSequence, DeltaTime);
        }
        else {
            self->UpdateAnimation(self->IdleSequence, DeltaTime);
        }
    };
    WalkCallback.func = [](UTestAnimInstance* self, float DeltaTime)
    {
        if (self->CurrentSequence != self->WalkSequence) {
            self->bIsBlending = true;
            self->BlendTime = 0.0f;
            self->PreviousSequence = self->CurrentSequence;
            self->CurrentSequence = self->WalkSequence;
        }
        if (self->bIsBlending) {
            self->BlendAnimations(self->PreviousSequence, self->CurrentSequence, DeltaTime);
        }
        else {
            self->UpdateAnimation(self->WalkSequence, DeltaTime);
        }
    };
    DanceCallback.func = [](UTestAnimInstance* self, float DeltaTime)
    {
        if (self->CurrentSequence != self->DanceSequence) {
            self->bIsBlending = true;
            self->BlendTime = 0.0f;
            self->PreviousSequence = self->CurrentSequence;
            self->CurrentSequence = self->DanceSequence;
        }
        if (self->bIsBlending) {
            self->BlendAnimations(self->PreviousSequence, self->CurrentSequence, DeltaTime);
        }
        else {
            self->UpdateAnimation(self->DanceSequence, DeltaTime);
        }
    };

    //**State 추가**
    AnimStateMachine->AddState(ETestState::Idle, IdleCallback);

    AnimStateMachine->AddState(ETestState::Walking, WalkCallback);

    AnimStateMachine->AddState(ETestState::Dancing, DanceCallback);

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
    CurrentSequence = IdleSequence;
    PreviousSequence = IdleSequence;
}

UTestAnimInstance::UTestAnimInstance(const UTestAnimInstance& Other) : 
    UAnimInstance(Other),
    IdleSequence(Other.IdleSequence),
    WalkSequence(Other.WalkSequence),
    DanceSequence(Other.DanceSequence)
{
    IdleCallback.func = Other.IdleCallback.func;
    WalkCallback.func = Other.WalkCallback.func;
    DanceCallback.func = Other.DanceCallback.func;
}

UObject* UTestAnimInstance::Duplicate(UObject* InOuter)
{
    UTestAnimInstance* NewComp = FObjectFactory::ConstructObjectFrom<UTestAnimInstance>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    NewComp->SetSkeleton(Cast<USkeletalMeshComponent>(InOuter)->GetSkeletalMesh()->GetSkeleton());
    return NewComp;
}

void UTestAnimInstance::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UAnimInstance::DuplicateSubObjects(Source, InOuter);
    UObject* obj = Cast<UTestAnimInstance>(Source)->AnimStateMachine->Duplicate(this);
    AnimStateMachine = Cast<UAnimationStateMachine<ETestState>>(obj);
}


void UTestAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (AnimStateMachine) AnimStateMachine->Update(DeltaSeconds);
}
