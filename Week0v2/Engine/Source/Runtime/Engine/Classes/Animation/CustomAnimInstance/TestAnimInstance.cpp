#include "TestAnimInstance.h"
#include "Animation/AnimationStateMachine.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimNotify/AnimNotify.h"
UTestAnimInstance::UTestAnimInstance()
{
    AnimStateMachine = FObjectFactory::ConstructObject<UAnimationStateMachine<ETestState>>(this);

    //**State 추가**
    AnimStateMachine->AddState(ETestState::Idle, [](float DeltaTime) {
        std::cout << "Idle Animation Playing..." << std::endl;
        });

    AnimStateMachine->AddState(ETestState::Walking, [](float DeltaTime) {
        std::cout << "Walking Animation Playing..." << std::endl;
        });

    AnimStateMachine->AddState(ETestState::Running, [](float DeltaTime) {
        std::cout << "Running Animation Playing..." << std::endl;
        });

    // **Transition Rule 정의**
    AnimStateMachine->AddTransition(ETestState::Idle, ETestState::Walking, [&]() { return Speed > 0 && Speed <= 300.0f; });
    AnimStateMachine->AddTransition(ETestState::Walking, ETestState::Running, [&]() { return Speed > 300.0f; });
    AnimStateMachine->AddTransition(ETestState::Running, ETestState::Idle, [&]() { return Speed == 0; });




    // 초기 상태 설정
    AnimStateMachine->SetState(ETestState::Idle);

    //IdleSequence = CreateAnimation("Idle.fbx");
    //WalkSequence = CreateAnimation("Walk.fbx");
    //RunSequence = CreateAnimation("Run.fbx");

}
UTestAnimInstance::~UTestAnimInstance()
{
}
void UTestAnimInstance::TriggerAnimNotifies(float DeltaSeconds) const
{
    if (CurrentSequence)
    {
        for (const FAnimNotifyEvent& Notify : CurrentSequence->Notifies)
        {
            // 시간 조건에 맞으면 Notify 실행
            if (Notify.TriggerTime <= CurrentTime && CurrentTime < Notify.TriggerTime + Notify.Duration)
            {
                if (Notify.Notify)
                {
                    Notify.Notify->Notify(GetSkelMeshComponent(), CurrentSequence);
                }
            }
        }
    }
}

void UTestAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    USkeletalMeshComponent* SkeletalMesh = GetSkelMeshComponent();
    AActor* OwnerPawn = SkeletalMesh->GetOwner();

    if (AnimStateMachine) AnimStateMachine->Update(DeltaSeconds);

    TriggerAnimNotifies(DeltaSeconds);
}
