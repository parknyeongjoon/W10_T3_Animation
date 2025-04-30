#include "USpringArmComponent.h"

USpringArmComponent::USpringArmComponent()
{
}

USpringArmComponent::USpringArmComponent(const USpringArmComponent& Other)
    : USceneComponent(Other)
    , TargetArmLength(Other.TargetArmLength)
    , SocketOffset(Other.SocketOffset)
    , TargetOffset(Other.TargetOffset)
    , bUseCollisionTest(Other.bUseCollisionTest)
    , ProbeSize(Other.ProbeSize)
    , bActiveCameraLag(Other.bActiveCameraLag)
    , CameraLagSpeed(Other.CameraLagSpeed)
    , CameraLagMaxDistance(Other.CameraLagMaxDistance)
    , bActiveCameraRotationLag(Other.bActiveCameraRotationLag)
    , CameraRotationLagSpeed(Other.CameraRotationLagSpeed)
    , CameraRotationLagMaxAngle(Other.CameraRotationLagMaxAngle)
{
}

USpringArmComponent::~USpringArmComponent()
{
}

void USpringArmComponent::InitializeComponent()
{
    Super::InitializeComponent();
    TargetArmLength = 10;
}

void USpringArmComponent::BeginPlay()
{
    // TargetComponent가 nullptr인 경우, AttachChildren의 첫 번째 자식으로 설정
    if (TargetComponent == nullptr && GetAttachChildren().Num() > 0)
    {
        TargetComponent = GetAttachChildren()[0];
    }    
}

void USpringArmComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    if (TargetComponent == nullptr)
    {
        return;
    }

    // Socket의 Local공간 위치 계산
    FVector TargetArmDirection = -FVector::ForwardVector;
    FVector SocketLocalLocation = TargetArmDirection * TargetArmLength;
    SocketLocalLocation += TargetOffset + SocketOffset;

    //TargetComponent->SetRelativeLocation(SocketLocalLocation);

    // Camera Rotation 적용
    FVector TargetRotation = TargetComponent->GetRelativeRotation().ToVector();
    TargetRotation.x *= 1;
    TargetRotation.y *= 1;
    TargetRotation.z *= 1;

    SetRelativeRotation(FRotator(TargetRotation));

}

void USpringArmComponent::DestroyComponent()
{
    Super::DestroyComponent();
}

UObject* USpringArmComponent::Duplicate() const
{
    Super::Duplicate();
    USpringArmComponent* NewComp = FObjectFactory::ConstructObjectFrom<USpringArmComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void USpringArmComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void USpringArmComponent::PostDuplicate()
{
    Super::PostDuplicate();
}

void USpringArmComponent::SetTargetComponent(USceneComponent* InTargetComponent)
{
    InTargetComponent->SetAttachParent(this);
    TargetComponent = InTargetComponent;
}


