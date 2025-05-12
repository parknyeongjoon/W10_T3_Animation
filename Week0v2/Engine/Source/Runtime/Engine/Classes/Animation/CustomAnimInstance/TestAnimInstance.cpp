#include "TestAnimInstance.h"
#include "Animation/AnimationStateMachine.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimNotify/AnimNotify.h"
UTestAnimInstance::UTestAnimInstance()
{

    IdleSequence->SetData("Contents/FBX/Idle.fbx\\mixamo.com");
    WalkSequence->SetData("Contents/FBX/Walking.fbx\\mixamo:com");
    DanceSequence->SetData("Contents/FBX/Rumba_Dancing.fbx\\mixamo:com");

    AnimStateMachine = FObjectFactory::ConstructObject<UAnimationStateMachine<ETestState>>(this);

    //**State 추가**
    AnimStateMachine->AddState(ETestState::Idle, [](float DeltaTime) {
        std::cout << "Idle Animation Playing..." << std::endl;
        });

    AnimStateMachine->AddState(ETestState::Walking, [](float DeltaTime) {
        std::cout << "Walking Animation Playing..." << std::endl;
        });

    AnimStateMachine->AddState(ETestState::Dancing, [](float DeltaTime) {
        std::cout << "Running Animation Playing..." << std::endl;
        });

    // **Transition Rule 정의**
    AnimStateMachine->AddTransition(ETestState::Idle, ETestState::Walking, [&]() { return Speed > 0 && Speed <= 300.0f; });
    AnimStateMachine->AddTransition(ETestState::Walking, ETestState::Dancing, [&]() { return Speed > 300.0f; });
    AnimStateMachine->AddTransition(ETestState::Dancing, ETestState::Idle, [&]() { return Speed == 0; });




    // 초기 상태 설정
    AnimStateMachine->SetState(ETestState::Idle);




}
UTestAnimInstance::~UTestAnimInstance()
{
}


void UTestAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    USkeletalMeshComponent* SkeletalMesh = GetOwningComponent();
    AActor* OwnerPawn = SkeletalMesh->GetOwner();

    if (AnimStateMachine) AnimStateMachine->Update(DeltaSeconds);

}
