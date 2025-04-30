#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
#include "ActorComponentInfo.h"

USceneComponent::USceneComponent() :RelativeLocation(FVector(0.f, 0.f, 0.f)), RelativeRotation(FVector(0.f, 0.f, 0.f)), RelativeScale(FVector(1.f, 1.f, 1.f))
{
}
USceneComponent::USceneComponent(const USceneComponent& Other)
    : UActorComponent(Other),
      AttachParent(nullptr), // 복제 시 복원
      RelativeLocation(Other.RelativeLocation),
      RelativeRotation(Other.RelativeRotation),
      RelativeScale(Other.RelativeScale),
      AABB(Other.AABB)
{
}
USceneComponent::~USceneComponent()
{
}
void USceneComponent::InitializeComponent()
{
   Super::InitializeComponent();
}

void USceneComponent::TickComponent(float DeltaTime)
{
	Super::TickComponent(DeltaTime);
}

void USceneComponent::DestroyComponent()
{
    TArray<USceneComponent*> ChildrenToDestroy = AttachChildren;
    
    for (USceneComponent* Child : ChildrenToDestroy)
    {
        // 자식 포인터가 유효한지 확인 (필수)
        if (Child)
        {
            Child->DestroyComponent(); // 재귀 호출
        }
    }
    if (AttachParent)
    {
        // 부모의 자식 목록에서 현재 컴포넌트 제거
        AttachParent->AttachChildren.Remove(this);
        // AttachParent = nullptr; // 어차피 파괴될 것이므로 필수는 아님
    }

    Super::DestroyComponent();
}


int USceneComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    int nIntersections = 0;
    return nIntersections;
}

FVector USceneComponent::GetForwardVector() const
{
    FVector Forward = FVector(1.f, 0.f, 0.0f);
    Forward = JungleMath::FVectorRotate(Forward, GetWorldRotation());
    return Forward;
}

FVector USceneComponent::GetRightVector() const
{
    FVector Right = FVector(0.f, 1.f, 0.0f);
    Right = JungleMath::FVectorRotate(Right, GetWorldRotation());
    return Right;
}

FVector USceneComponent::GetUpVector() const
{
    FVector Up = FVector(0.f, 0.f, 1.0f);
    Up = JungleMath::FVectorRotate(Up, GetWorldRotation());
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
	RelativeScale = RelativeScale + _added;
}

void USceneComponent::AttachToComponent(USceneComponent* InParent)
{
    // 기존 부모와 연결을 끊기
    if (AttachParent)
    {
        AttachParent->AttachChildren.Remove(this);
    }

    // InParent도 nullptr이면 부모를 nullptr로 설정
    if (InParent == nullptr)
    {
        AttachParent = nullptr;
        return;
    }


    // 새로운 부모 설정
    AttachParent = InParent;

    // 부모의 자식 리스트에 추가
    if (!InParent->AttachChildren.Contains(this))
    {
        InParent->AttachChildren.Add(this);
    }
}

FVector USceneComponent::GetWorldLocation() const
{
    if (AttachParent)
    {
        return AttachParent->GetWorldLocation() + RelativeLocation;
    }
    return RelativeLocation;
}

FRotator USceneComponent::GetWorldRotation() const
{
    if (AttachParent)
    {
        return AttachParent->GetWorldRotation().ToQuaternion() * RelativeRotation.ToQuaternion();
    }
    return RelativeRotation;
}

FVector USceneComponent::GetWorldScale() const
{
    if (AttachParent)
    {
        return AttachParent->GetWorldScale() * RelativeScale;
    }
    return RelativeScale;
}

FMatrix USceneComponent::GetScaleMatrix() const
{
    FMatrix ScaleMat = FMatrix::GetScaleMatrix(RelativeScale);
    if (AttachParent)
    {
        FMatrix ParentScaleMat = AttachParent->GetScaleMatrix();
        ScaleMat = ScaleMat * ParentScaleMat;
    }
    return ScaleMat;
}

FMatrix USceneComponent::GetRotationMatrix() const
{
    FMatrix RotationMat = FMatrix::GetRotationMatrix(RelativeRotation.ToQuaternion());
    if (AttachParent)
    {
        FMatrix ParentRotationMat = AttachParent->GetRotationMatrix();
        RotationMat = RotationMat * ParentRotationMat;
    }
    return RotationMat;
}

FMatrix USceneComponent::GetTranslationMatrix() const
{
    FMatrix TranslationMat = FMatrix::GetTranslationMatrix(RelativeLocation);
    if (AttachParent)
    {
        FMatrix ParentTranslationMat = AttachParent->GetTranslationMatrix();
        TranslationMat = TranslationMat * ParentTranslationMat;
    }
    return TranslationMat;
}

FMatrix USceneComponent::GetWorldMatrix() const
{
    FMatrix ScaleMat = FMatrix::GetScaleMatrix(RelativeScale);
    FMatrix RotationMat = FMatrix::GetRotationMatrix(RelativeRotation.ToQuaternion());
    FMatrix TranslationMat = FMatrix::GetTranslationMatrix(RelativeLocation);

    FMatrix RTMat = RotationMat * TranslationMat;
    if (AttachParent)
    {
        FMatrix ParentScaleMat = AttachParent->GetScaleMatrix();
        FMatrix ParentRotationMat = AttachParent->GetRotationMatrix();
        FMatrix ParentTranslationMat = AttachParent->GetTranslationMatrix();
        
        ScaleMat = ScaleMat * ParentScaleMat;
        FMatrix ParentRTMat = ParentRotationMat * ParentTranslationMat;
        RTMat = RTMat * ParentRTMat;
    }
    return ScaleMat * RTMat;
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

void USceneComponent::DetachFromParent()
{
    if (AttachParent)
    {
        AttachParent->AttachChildren.Remove(this);
        AttachParent = nullptr;
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

std::unique_ptr<FActorComponentInfo> USceneComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FSceneComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void USceneComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FSceneComponentInfo& Info = static_cast<FSceneComponentInfo&>(OutInfo);
    Super::SaveComponentInfo(Info);
    Info.RelativeLocation = RelativeLocation;
    Info.RelativeRotation = RelativeRotation;
    Info.RelativeScale3D = RelativeScale;
    Info.AABB = AABB;

    // 부모 ID 저장
    if (AttachParent)
    {
        Info.ParentComponentID = AttachParent->GetComponentID(); // 부모의 ID 가져오기
    }
    else
    {
        Info.ParentComponentID = FGuid(); // 부모 없음을 표시 (기본 Guid)
    }
}

void USceneComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FSceneComponentInfo& SceneInfo = static_cast<const FSceneComponentInfo&>(Info);
    RelativeLocation = SceneInfo.RelativeLocation;
    RelativeRotation = SceneInfo.RelativeRotation;
    RelativeScale = SceneInfo.RelativeScale3D;
    AABB = SceneInfo.AABB;

    // 부모 ID를 임시 변수에 저장
    PendingAttachParentID = SceneInfo.ParentComponentID;
}
