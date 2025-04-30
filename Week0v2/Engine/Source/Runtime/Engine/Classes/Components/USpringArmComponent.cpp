#include "USpringArmComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponents/Physics/USphereShapeComponent.h"

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

    if (ChangeTargetArmLength != TargetArmLength)
    {
        TargetArmLength = FMath::Lerp(TargetArmLength, ChangeTargetArmLength, 30 * DeltaTime * 0.001f);
        if (FMath::Abs(TargetArmLength - ChangeTargetArmLength) < 0.01f)
        {
            TargetArmLength = ChangeTargetArmLength;
        }
    }

    // Socket의 Local공간 위치 계산
    FVector TargetArmDirection = -FVector::ForwardVector + FVector::UpVector * 0.5f;
    TargetArmDirection = TargetArmDirection.Normalize();
    FVector SocketLocalLocation = TargetArmDirection * TargetArmLength;
    SocketLocalLocation += TargetOffset + SocketOffset;

    if (bActiveCameraLag)
    {
        FVector SocketWorldLocation = AttachParent->GetWorldMatrix().TransformPosition(SocketLocalLocation);
        FVector CurrentWorldLocation = GetWorldLocation();

        FVector LaggedWorldLocation = FMath::Lerp<FVector>(CurrentWorldLocation, SocketWorldLocation, CameraLagSpeed);
        SetWorldLocation(LaggedWorldLocation);
    }
    else
    {
        SetRelativeLocation(SocketLocalLocation);
    }

    //TargetComponent->SetRelativeLocation(SocketLocalLocation);
    //TargetComponent->SetWorldLocation(FVector(0, 0, 10));
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


