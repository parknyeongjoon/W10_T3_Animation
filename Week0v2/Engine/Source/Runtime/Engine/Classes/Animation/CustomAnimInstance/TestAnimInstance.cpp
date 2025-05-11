#include "TestAnimInstance.h"
#include "Animation/AnimationStateMachine.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
UTestAnimInstance::UTestAnimInstance()
{
    AnimStateMachine = FObjectFactory::ConstructObject<UAnimationStateMachine>(this);

    //**State 추가**
    AnimStateMachine->AddState(TEXT("Idle"), [](float DeltaTime) {
        std::cout << "Idle Animation Playing..." << std::endl;
        });

    AnimStateMachine->AddState(TEXT("Walking"), [](float DeltaTime) {
        std::cout << "Walking Animation Playing..." << std::endl;
        });

    AnimStateMachine->AddState(TEXT("Running"), [](float DeltaTime) {
        std::cout << "Running Animation Playing..." << std::endl;
        });

    // **Transition Rule 정의**
    AnimStateMachine->AddTransition(TEXT("Idle"), TEXT("Walking"), [&]() { return Speed > 0 && Speed <= 300.0f; });
    AnimStateMachine->AddTransition(TEXT("Walking"), TEXT("Running"), [&]() { return Speed > 300.0f; });
    AnimStateMachine->AddTransition(TEXT("Running"), TEXT("Idle"), [&]() { return Speed == 0; });




    // 초기 상태 설정
    AnimStateMachine->SetState(TEXT("Idle"));

    //IdleSequence = CreateAnimation("Idle.fbx");
    //WalkSequence = CreateAnimation("Walk.fbx");
    //RunSequence = CreateAnimation("Run.fbx");

}
UTestAnimInstance::~UTestAnimInstance()
{
}
void UTestAnimInstance::TriggerAnimNotifies(float DeltaSeconds)
{
}

void UTestAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    USkeletalMeshComponent* SkeletalMesh = GetSkelMeshComponent();
    AActor* OwnerPawn = SkeletalMesh->GetOwner();

    if (AnimStateMachine) AnimStateMachine->Update(DeltaSeconds);

}
