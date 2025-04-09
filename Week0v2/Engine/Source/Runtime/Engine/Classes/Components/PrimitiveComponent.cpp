#include "PrimitiveComponent.h"

#include "GameFramework/Actor.h"
#include "UObject/ObjectFactory.h"

UPrimitiveComponent::UPrimitiveComponent()
{
}


UPrimitiveComponent::UPrimitiveComponent(const UPrimitiveComponent& Other)
    : USceneComponent(Other)
    , AABB(Other.AABB)
{
}


UPrimitiveComponent::~UPrimitiveComponent()
{
}

void UPrimitiveComponent::InitializeComponent()
{
    // Super::InitializeComponent();
}

void UPrimitiveComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
    
}

int UPrimitiveComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    //if (!AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance)) return 0;
    int nIntersections = 0;
    //if (staticMesh == nullptr) return 0;
    //FVertexSimple* vertices = staticMesh->vertices.get();
    //int vCount = staticMesh->numVertices;
    //UINT* indices = staticMesh->indices.get();
    //int iCount = staticMesh->numIndices;

    //if (!vertices) return 0;
    //BYTE* pbPositions = reinterpret_cast<BYTE*>(staticMesh->vertices.get());
    //
    //int nPrimitives = (!indices) ? (vCount / 3) : (iCount / 3);
    //float fNearHitDistance = FLT_MAX;
    //for (int i = 0; i < nPrimitives; i++) {
    //    int idx0, idx1, idx2;
    //    if (!indices) {
    //        idx0 = i * 3;
    //        idx1 = i * 3 + 1;
    //        idx2 = i * 3 + 2;
    //    }
    //    else {
    //        idx0 = indices[i * 3];
    //        idx2 = indices[i * 3 + 1];
    //        idx1 = indices[i * 3 + 2];
    //    }

    //    // 각 삼각형의 버텍스 위치를 FVector로 불러옵니다.
    //    uint32 stride = sizeof(FVertexSimple);
    //    FVector v0 = *reinterpret_cast<FVector*>(pbPositions + idx0 * stride);
    //    FVector v1 = *reinterpret_cast<FVector*>(pbPositions + idx1 * stride);
    //    FVector v2 = *reinterpret_cast<FVector*>(pbPositions + idx2 * stride);

    //    float fHitDistance;
    //    if (IntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, fHitDistance)) {
    //        if (fHitDistance < fNearHitDistance) {
    //            pfNearHitDistance =  fNearHitDistance = fHitDistance;
    //        }
    //        nIntersections++;
    //    }
    //   
    //}
    return nIntersections;
}

bool UPrimitiveComponent::IntersectRayTriangle(const FVector& rayOrigin, const FVector& rayDirection, const FVector& v0, const FVector& v1,
                                               const FVector& v2, float& hitDistance)
{
    const float epsilon = 1e-6f;
    FVector edge1 = v1 - v0;
    const FVector edge2 = v2 - v0;
    FVector FrayDirection = rayDirection;
    FVector h = FrayDirection.Cross(edge2);
    float a = edge1.Dot(h);

    if (fabs(a) < epsilon)
        return false; // Ray와 삼각형이 평행한 경우

    float f = 1.0f / a;
    FVector s = rayOrigin - v0;
    float u = f * s.Dot(h);
    if (u < 0.0f || u > 1.0f)
        return false;

    FVector q = s.Cross(edge1);
    float v = f * FrayDirection.Dot(q);
    if (v < 0.0f || (u + v) > 1.0f)
        return false;

    float t = f * edge2.Dot(q);
    if (t > epsilon)
    {
        hitDistance = t;
        return true;
    }

    return false;
}

UObject* UPrimitiveComponent::Duplicate() const
{
    UPrimitiveComponent* NewComp = FObjectFactory::ConstructObjectFrom<UPrimitiveComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void UPrimitiveComponent::DuplicateSubObjects(const UObject* Source)
{
    USceneComponent::DuplicateSubObjects(Source);
}

void UPrimitiveComponent::PostDuplicate()
{
    USceneComponent::PostDuplicate();
}

bool UPrimitiveComponent::MoveComponent(const FVector& Delta)
{
    if (!GetOwner()) return false;
    
    FVector NewLocation = GetOwner()->GetActorLocation() + Delta;
    GetOwner()->SetActorLocation(NewLocation);

    return true;
}

std::shared_ptr<FActorComponentInfo> UPrimitiveComponent::GetActorComponentInfo()
{
    std::shared_ptr<FPrimitiveComponentInfo> Info = std::make_shared<FPrimitiveComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);

    Info->AABB = AABB;

    return Info;
}

void UPrimitiveComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FPrimitiveComponentInfo* PrimitiveInfo = static_cast<const FPrimitiveComponentInfo*>(&Info);
    AABB = PrimitiveInfo->AABB;
}
