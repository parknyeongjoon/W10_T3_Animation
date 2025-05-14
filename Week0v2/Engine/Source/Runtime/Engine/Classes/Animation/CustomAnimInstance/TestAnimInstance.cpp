#include "TestAnimInstance.h"
#include "Animation/AnimationStateMachine.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimNotify/AnimNotify.h"
#include "Components/GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"

UTestAnimInstance::UTestAnimInstance()
{
    StandingSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);
    JumpSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);
    DanceSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);
    DeafeatedSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);

    FFBXLoader::CreateSkeletalMesh("Contents/FBX/Cute_Standing_Pose.fbx");
    FFBXLoader::CreateSkeletalMesh("Contents/FBX/Joyful_Jump.fbx");
    FFBXLoader::CreateSkeletalMesh("Contents/FBX/Rumba_Dancing.fbx");
    FFBXLoader::CreateSkeletalMesh("Contents/FBX/Defeated.fbx");

    StandingSequence->SetData("Contents/FBX/Cute_Standing_Pose.fbx\\mixamo.com");
    JumpSequence->SetData("Contents/FBX/Joyful_Jump.fbx\\mixamo.com");
    DanceSequence->SetData("Contents/FBX/Rumba_Dancing.fbx\\mixamo.com");
    DeafeatedSequence->SetData("Contents/FBX/Defeated.fbx\\mixamo.com");

    AnimStateMachine = FObjectFactory::ConstructObject<UAnimationStateMachine<ETestState>>(this);

    StandingCallback.func = [](UTestAnimInstance* self, float DeltaTime)
    {
        if (self->CurrentSequence != self->StandingSequence) {
            self->bIsBlending = true;
            self->PreviousSequence = self->CurrentSequence;
            self->CurrentSequence = self->StandingSequence;
            self->CurrentSequence->ResetNotifies();
            self->CurrentTime = 0.0f;
            self->BlendTime = 0.0f;
            self->CapturePose();
        }
        if (self->bIsBlending) {
            self->BlendAnimations(self->PreviousSequence, self->CurrentSequence, DeltaTime);
        }
        else {
            self->UpdateAnimation(self->StandingSequence, DeltaTime);
        }
    };
    JumpCallback.func = [](UTestAnimInstance* self, float DeltaTime)
    {
        if (self->CurrentSequence != self->JumpSequence) {
            self->bIsBlending = true;
            self->PreviousSequence = self->CurrentSequence;
            self->CurrentSequence = self->JumpSequence;
            self->CurrentSequence->ResetNotifies();
            self->CurrentTime = 0.0f;
            self->BlendTime = 0.0f;
            self->CapturePose();
        }
        if (self->bIsBlending) {
            self->BlendAnimations(self->PreviousSequence, self->CurrentSequence, DeltaTime);
        }
        else {
            self->UpdateAnimation(self->JumpSequence, DeltaTime);
        }
    };
    DanceCallback.func = [](UTestAnimInstance* self, float DeltaTime)
    {
        if (self->CurrentSequence != self->DanceSequence) {
            self->bIsBlending = true;
            self->PreviousSequence = self->CurrentSequence;
            self->CurrentSequence = self->DanceSequence;
            self->CurrentSequence->ResetNotifies();
            self->CurrentTime = 0.0f;
            self->BlendTime = 0.0f;
            self->CapturePose();
        }
        if (self->bIsBlending) {
            self->BlendAnimations(self->PreviousSequence, self->CurrentSequence, DeltaTime);
        }
        else {
            self->UpdateAnimation(self->DanceSequence, DeltaTime);
        }
    };

    DeafeatedCallback.func = [](UTestAnimInstance* self, float DeltaTime)
    {
        if (self->CurrentSequence != self->DeafeatedSequence) {
            self->bIsBlending = true;
            self->BlendTime = 0.0f;
            self->PreviousSequence = self->CurrentSequence;
            self->CurrentSequence = self->DeafeatedSequence;
            self->CurrentSequence->ResetNotifies();
            self->CurrentTime = 0.0f;
            self->CapturePose();
        }
        if (self->bIsBlending) {
            self->BlendAnimations(self->PreviousSequence, self->CurrentSequence, DeltaTime);
        }
        else {
            self->UpdateAnimation(self->DeafeatedSequence, DeltaTime);
        }
    };

    //**State 추가**
    AnimStateMachine->AddState(ETestState::Pose, StandingCallback);

    AnimStateMachine->AddState(ETestState::Jump, JumpCallback);

    AnimStateMachine->AddState(ETestState::Dance, DanceCallback);

    AnimStateMachine->AddState(ETestState::Defeated, DeafeatedCallback);

    // **Transition Rule 정의**
    // AnimStateMachine->AddTransition(ETestState::Jump, ETestState::Dance, [&]() {
    //     ACharacter* Character = Cast<ACharacter>(GetOwningActor());
    //     if (Character == nullptr) return false;
    //     return Character->GetMovementComponent()->Velocity.Magnitude() < 0.1f;
    //     });
    //
    // AnimStateMachine->AddTransition(ETestState::Pose, ETestState::Jump, [&]() {
    //     ACharacter* Character = Cast<ACharacter>(GetOwningActor());
    //     if (Character == nullptr) return false;
    //     return Character->GetMovementComponent()->Velocity.Magnitude() >= 0.1f;
    //     });
    //
    // AnimStateMachine->AddTransition(ETestState::Dance, ETestState::Jump, [&]() {
    // ACharacter* Character = Cast<ACharacter>(GetOwningActor());
    // if (Character == nullptr) return false;
    // return Character->GetMovementComponent()->Velocity.Magnitude() >= 0.1f;
    // });

    // 초기 상태 설정
    AnimStateMachine->SetState(ETestState::Dance);
    CurrentSequence = DanceSequence;
    PreviousSequence = DanceSequence;
    
    StandingSequence->AddNotify(1.0f, []()
    {
        printf("AnimNotify: Standing\n");
    });
    JumpSequence->AddNotify(1.0f, []()
    {
        printf("AnimNotify: Jump\n");
    });
    DanceSequence->AddNotify(1.0f, []()
    {
        printf("AnimNotify: Dancing\n");
    });
    DeafeatedSequence->AddNotify(1.0f, []()
{
    printf("AnimNotify: Defeated\n");
});

    CurrentSequence->GetAnimationPose(CurrentPose, FAnimExtractContext(0.0f, true, false));
}

UTestAnimInstance::UTestAnimInstance(const UTestAnimInstance& Other) : 
    UAnimInstance(Other),
    StandingSequence(Other.StandingSequence),
    JumpSequence(Other.JumpSequence),
    DanceSequence(Other.DanceSequence),
    DeafeatedSequence(Other.DeafeatedSequence)
{
    StandingCallback.func = Other.StandingCallback.func;
    JumpCallback.func = Other.JumpCallback.func;
    DanceCallback.func = Other.DanceCallback.func;

    CurrentSequence->GetAnimationPose(CurrentPose, FAnimExtractContext(0.0f, true, false));
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


void UTestAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (AnimStateMachine) AnimStateMachine->Update(DeltaSeconds);
}
