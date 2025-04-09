#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
#include "UTextUUID.h"
#include "ActorComponentInfo.h"

USceneComponent::USceneComponent() :RelativeLocation(FVector(0.f, 0.f, 0.f)), RelativeRotation(FVector(0.f, 0.f, 0.f)), RelativeScale3D(FVector(1.f, 1.f, 1.f))
{
}
USceneComponent::USceneComponent(const USceneComponent& Other)
    : UActorComponent(Other),
      AttachParent(nullptr), // 복제 시 복원
      RelativeLocation(Other.RelativeLocation),
      RelativeRotation(Other.RelativeRotation),
      QuatRotation(Other.QuatRotation),
      RelativeScale3D(Other.RelativeScale3D)
{
}
USceneComponent::~USceneComponent()
{
	if (uuidText) delete uuidText;
}
void USceneComponent::InitializeComponent()
{
   // Super::InitializeComponent();

}

void USceneComponent::TickComponent(float DeltaTime)
{
	Super::TickComponent(DeltaTime);
}


int USceneComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    int nIntersections = 0;
    return nIntersections;
}

FVector USceneComponent::GetForwardVector()
{
	FVector Forward = FVector(1.f, 0.f, 0.0f);
	Forward = JungleMath::FVectorRotate(Forward, QuatRotation);
	return Forward;
}

FVector USceneComponent::GetRightVector()
{
	FVector Right = FVector(0.f, 1.f, 0.0f);
	Right = JungleMath::FVectorRotate(Right, QuatRotation);
	return Right;
}

FVector USceneComponent::GetUpVector()
{
	FVector Up = FVector(0.f, 0.f, 1.0f);
	Up = JungleMath::FVectorRotate(Up, QuatRotation);
	return Up;
}


void USceneComponent::AddLocation(FVector _added)
{
	RelativeLocation = RelativeLocation + _added;

}

void USceneComponent::AddRotation(FVector _added)
{
	RelativeRotation = RelativeRotation + _added;

}

void USceneComponent::AddScale(FVector _added)
{
	RelativeScale3D = RelativeScale3D + _added;

}

FVector USceneComponent::GetWorldRotation()
{
	if (AttachParent)
	{
		return FVector(AttachParent->GetLocalRotation() + GetLocalRotation());
	}
	else
		return GetLocalRotation();
}

FVector USceneComponent::GetWorldScale()
{
	if (AttachParent)
	{
		return FVector(AttachParent->GetWorldScale() * GetLocalScale());
	}
	else
		return GetLocalScale();
}

FVector USceneComponent::GetWorldLocation()
{
	if (AttachParent)
	{
		return FVector(AttachParent->GetWorldLocation() + GetLocalLocation());
	}
	else
		return GetLocalLocation();
}

FVector USceneComponent::GetLocalRotation()
{
	return JungleMath::QuaternionToEuler(QuatRotation);
}

void USceneComponent::SetRotation(FVector _newRot)
{
	RelativeRotation = _newRot;
	QuatRotation = JungleMath::EulerToQuaternion(_newRot);
}

void USceneComponent::SetupAttachment(USceneComponent* InParent)
{
    if (
        InParent != AttachParent                                  // 설정하려는 Parent가 기존의 Parent와 다르거나
        && InParent != this                                       // InParent가 본인이 아니고
        && InParent != nullptr                                    // InParent가 유효한 포인터 이며
        && (
            AttachParent == nullptr                               // AttachParent도 유효하며
            || !AttachParent->AttachChildren.Contains(this)  // 이미 AttachParent의 자식이 아닌 경우
        ) 
    ) {
        SetAttachParent(InParent);
        InParent->AttachChildren.AddUnique(this);
    }
}


USceneComponent* USceneComponent::GetAttachParent() const
{
    return AttachParent;
}

void USceneComponent::SetAttachParent(USceneComponent* InParent)
{
    AttachParent = InParent;
}

UObject* USceneComponent::Duplicate() const
{
    USceneComponent* NewComp = FObjectFactory::ConstructObjectFrom<USceneComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void USceneComponent::DuplicateSubObjects(const UObject* Source)
{
    UActorComponent::DuplicateSubObjects(Source);
    // AttachParent는 AActor::DuplicateSubObjects에서 복원
}

void USceneComponent::PostDuplicate() {}

std::shared_ptr<FActorComponentInfo> USceneComponent::GetActorComponentInfo()
{
    std::shared_ptr<FSceneComponentInfo> Info = std::make_shared<FSceneComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);

    Info->InfoType = GetClass()->GetName();
    Info->RelativeLocation = RelativeLocation;
    Info->RelativeRotation = RelativeRotation;
    Info->RelativeScale3D = RelativeScale3D;
    Info->QuatRotation = QuatRotation;

    // !TODO : AttachedParent
    return Info;
}

void USceneComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FSceneComponentInfo& SceneInfo = static_cast<const FSceneComponentInfo&>(Info);
    RelativeLocation = SceneInfo.RelativeLocation;
    RelativeRotation = SceneInfo.RelativeRotation;
    QuatRotation = SceneInfo.QuatRotation;
    RelativeScale3D = SceneInfo.RelativeScale3D;
}
