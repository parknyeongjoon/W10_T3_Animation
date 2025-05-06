#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
#include "ActorComponentInfo.h"
//#include "Math/Matrix.h"

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

UObject* USceneComponent::Duplicate(UObject* InOuter)
{
    USceneComponent* NewComp = FObjectFactory::ConstructObjectFrom<USceneComponent>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

void USceneComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UActorComponent::DuplicateSubObjects(Source, InOuter);
    // AttachParent는 AActor::DuplicateSubObjects에서 복원
}

FVector USceneComponent::GetWorldForwardVector() const
{
    FVector Forward = FVector::ForwardVector;
    Forward = JungleMath::FVectorRotate(Forward, GetWorldRotation());
    return Forward;
}

FVector USceneComponent::GetWorldRightVector() const
{
    FVector Right = FVector::RightVector;
    Right = JungleMath::FVectorRotate(Right, GetWorldRotation());
    return Right;
}

FVector USceneComponent::GetWorldUpVector() const
{
    FVector Up = FVector::UpVector;
    Up = JungleMath::FVectorRotate(Up, GetWorldRotation());
    return Up;
}

FVector USceneComponent::GetRelativeForwardVector() const
{
    FVector Forward = FVector::ForwardVector;
    Forward = JungleMath::FVectorRotate(Forward, GetRelativeRotation());
    return Forward;
}

FVector USceneComponent::GetRelativeRightVector() const
{
    FVector Right = FVector::RightVector;
    Right = JungleMath::FVectorRotate(Right, GetRelativeRotation());
    return Right;
}

FVector USceneComponent::GetRelativeUpVector() const
{
    FVector Up = FVector::UpVector;
    Up = JungleMath::FVectorRotate(Up, GetRelativeRotation());
    return Up;
}

void USceneComponent::AddRelativeLocation(const FVector& InAddValue)
{
    RelativeLocation = RelativeLocation + InAddValue;
}

void USceneComponent::AddRelativeRotation(const FRotator& InAddValue)
{
    RelativeRotation = RelativeRotation + InAddValue;
    RelativeRotation.Normalize();
}

void USceneComponent::AddRelativeScale(const FVector& InAddValue)
{
    RelativeScale = RelativeScale + InAddValue;
}

void USceneComponent::SetRelativeLocation(const FVector& InLocation)
{
    RelativeLocation = InLocation;
}

void USceneComponent::SetRelativeRotation(const FRotator& InRotation)
{
    SetRelativeRotation(InRotation.ToQuaternion());
}

void USceneComponent::SetRelativeRotation(const FQuat& InQuat)
{
    FQuat NormalizedQuat = InQuat.GetSafeNormal();
    RelativeRotation = NormalizedQuat.Rotator();
    RelativeRotation.Normalize();
}

void USceneComponent::SetRelativeScale(const FVector& InScale)
{
    RelativeScale = InScale;
}

FVector USceneComponent::GetRelativeLocation() const
{
    return RelativeLocation;
}

FRotator USceneComponent::GetRelativeRotation() const
{
    return RelativeRotation;
}

FVector USceneComponent::GetRelativeScale() const
{
    return RelativeScale;
}

FVector USceneComponent::GetWorldLocation() const
{
    return GetWorldMatrix().GetTranslationVector();
}

FRotator USceneComponent::GetWorldRotation() const
{
    FMatrix WorldMatrix = GetWorldMatrix().GetMatrixWithoutScale();
    FQuat Quat = WorldMatrix.ToQuat();
    return FRotator(Quat);
}

FVector USceneComponent::GetWorldScale() const
{
    return GetWorldMatrix().GetScaleVector();
}

void USceneComponent::AddWorldLocation(const FVector& InAddValue)
{
    SetWorldLocation(GetWorldLocation() + InAddValue);
}

void USceneComponent::AddWorldRotation(const FRotator& InAddValue)
{
    SetWorldRotation(GetWorldRotation() + InAddValue);
}

void USceneComponent::AddWorldScale(const FVector& InAddValue)
{
    SetWorldScale(GetWorldScale() + InAddValue);
}

void USceneComponent::SetWorldLocation(const FVector& InLocation)
{
    // TODO: 코드 최적화 방법 생각하기
    FMatrix NewRelativeMatrix = FMatrix::CreateTranslationMatrix(InLocation);
    if (AttachParent)
    {
        FMatrix ParentMatrix = AttachParent->GetWorldMatrix().GetMatrixWithoutScale();
        NewRelativeMatrix = NewRelativeMatrix * FMatrix::Inverse(ParentMatrix);
    }
    FVector NewRelativeLocation = NewRelativeMatrix.GetTranslationVector();
    RelativeLocation = NewRelativeLocation;
}

void USceneComponent::SetWorldRotation(const FRotator& InRotation)
{
    SetWorldRotation(InRotation.ToQuaternion());
}

void USceneComponent::SetWorldRotation(const FQuat& InQuat)
{
    // TODO: 코드 최적화 방법 생각하기
    FMatrix NewRelativeMatrix = InQuat.ToMatrix();
    if (AttachParent)
    {
        FMatrix ParentMatrix = AttachParent->GetWorldMatrix().GetMatrixWithoutScale();
        NewRelativeMatrix = NewRelativeMatrix * FMatrix::Inverse(ParentMatrix);
    }
    FQuat NewRelativeRotation = FQuat(NewRelativeMatrix);
    RelativeRotation = FRotator(NewRelativeRotation);
    RelativeRotation.Normalize();   
}

void USceneComponent::SetWorldScale(const FVector& InScale)
{
    // TODO: 코드 최적화 방법 생각하기
    FMatrix NewRelativeMatrix = FMatrix::CreateScaleMatrix(InScale.X, InScale.Y, InScale.Z);
    if (AttachParent)
    {
        FMatrix ParentMatrix = FMatrix::GetScaleMatrix(AttachParent->RelativeScale);
        NewRelativeMatrix = NewRelativeMatrix * FMatrix::Inverse(ParentMatrix);
    }
    FVector NewRelativeScale = NewRelativeMatrix.GetScaleVector();
    RelativeScale = NewRelativeScale;
}

FMatrix USceneComponent::GetScaleMatrix() const
{
    return FMatrix::GetScaleMatrix(RelativeScale);
}

FMatrix USceneComponent::GetRotationMatrix() const
{
    return FMatrix::GetRotationMatrix(RelativeRotation);
}

FMatrix USceneComponent::GetTranslationMatrix() const
{
    return FMatrix::GetTranslationMatrix(RelativeLocation);
}

FMatrix USceneComponent::GetWorldMatrix() const
{
    FMatrix ScaleMat = GetScaleMatrix();
    FMatrix RotationMat = GetRotationMatrix();
    FMatrix TranslationMat = GetTranslationMatrix();

    FMatrix RTMat = RotationMat * TranslationMat;
    USceneComponent* Parent = AttachParent;
    while (Parent)
    {
        FMatrix ParentScaleMat = Parent->GetScaleMatrix();
        FMatrix ParentRotationMat = Parent->GetRotationMatrix();
        FMatrix ParentTranslationMat = Parent->GetTranslationMatrix();
        
        ScaleMat = ScaleMat * ParentScaleMat;
        FMatrix ParentRTMat = ParentRotationMat * ParentTranslationMat;
        RTMat = RTMat * ParentRTMat;

        Parent = Parent->AttachParent;
    }
    return ScaleMat * RTMat;
}

FMatrix USceneComponent::GetWorldRTMatrix() const
{
    FMatrix RotationMat = FMatrix::GetRotationMatrix(RelativeRotation);
    FMatrix TranslationMat = FMatrix::GetTranslationMatrix(RelativeLocation);

    FMatrix RTMat = RotationMat * TranslationMat;

    if (AttachParent)
    {
        FMatrix ParentRotationMat = AttachParent->GetRotationMatrix();
        FMatrix ParentTranslationMat = AttachParent->GetTranslationMatrix();
        FMatrix ParentRTMat = ParentRotationMat * ParentTranslationMat;

        RTMat = RTMat * ParentRTMat;
    }

    return RTMat;
}

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
