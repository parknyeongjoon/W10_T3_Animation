#include "UBoxShapeComponent.h"
#include "Math/JungleMath.h"
#include "USphereShapeComponent.h"
#include "UCapsuleShapeComponent.h"

UBoxShapeComponent::UBoxShapeComponent()
    : UShapeComponent()
    , BoxExtent(FVector::OneVector) // Default box extent
{
}

UBoxShapeComponent::UBoxShapeComponent(const UBoxShapeComponent& Other)
    : UShapeComponent(Other)
    , BoxExtent(Other.BoxExtent) 
{
}

UBoxShapeComponent::~UBoxShapeComponent()
{
    
}

void UBoxShapeComponent::InitializeComponent()
{
    Super::InitializeComponent();

    UpdateBroadAABB();
}

void UBoxShapeComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void UBoxShapeComponent::UpdateBroadAABB()
{
    FVector Center = GetComponentLocation();
    FVector Scale = GetComponentScale();
    FVector Extent = BoxExtent * Scale /** 0.5f*/;

    FVector LocalCorners[8] =
    {
        FVector(-Extent.x, -Extent.y, -Extent.z),
        FVector(Extent.x, -Extent.y, -Extent.z),
        FVector(Extent.x,  Extent.y, -Extent.z),
        FVector(-Extent.x,  Extent.y, -Extent.z),
        FVector(-Extent.x, -Extent.y,  Extent.z),
        FVector(Extent.x, -Extent.y,  Extent.z),
        FVector(Extent.x,  Extent.y,  Extent.z),
        FVector(-Extent.x,  Extent.y,  Extent.z)
    };

    FMatrix RotationMatrix = GetRotationMatrix();

    FVector Min = FVector::ZeroVector;
    FVector Max = FVector::ZeroVector;
    
    for (int i = 0; i < 8; ++i)
    {
        FVector WorldCorner = FMatrix::TransformVector(LocalCorners[i], RotationMatrix);

        Min.x = FMath::Min(Min.x, WorldCorner.x);
        Min.y = FMath::Min(Min.y, WorldCorner.y);
        Min.z = FMath::Min(Min.z, WorldCorner.z);

        Max.x = FMath::Max(Max.x, WorldCorner.x);
        Max.y = FMath::Max(Max.y, WorldCorner.y);
        Max.z = FMath::Max(Max.z, WorldCorner.z);
    }

    BroadAABB.min = Center + Min;
    BroadAABB.max = Center + Max;
}

bool UBoxShapeComponent::TestOverlaps(const UShapeComponent* OtherShape) const
{
    if (!BroadPhaseCollisionCheck(OtherShape))
    {
        return false;
    }

    return NarrowPhaseCollisionCheck(OtherShape);
}

bool UBoxShapeComponent::NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const
{
    if (!OtherShape)
    {
        return false;
    }

    if (OtherShape->IsA<UBoxShapeComponent>())
    {
        const UBoxShapeComponent* OtherBox = dynamic_cast<const UBoxShapeComponent*>(OtherShape);
        return CollisionCheckWithBox(OtherBox);
    }
    else if (OtherShape->IsA<USphereShapeComponent>()) 
    {
        const USphereShapeComponent* OtherSphere = dynamic_cast<const USphereShapeComponent*>(OtherShape);
        return CollisionCheckWithSphere(OtherSphere);
    }
    else if (OtherShape->IsA<UCapsuleShapeComponent>())
    {
        const UCapsuleShapeComponent* OtherCapsule = dynamic_cast<const UCapsuleShapeComponent*>(OtherShape);
        return CollisionCheckWithCapsule(OtherCapsule);
    }

    return false;
}

// OBB vs OBB
bool UBoxShapeComponent::CollisionCheckWithBox(const UBoxShapeComponent* OtherBox) const
{
    // 1) 자신의 OBB 정보
    FVector CenterA = GetComponentLocation();
    FVector ExtentA = GetBoxExtent() * GetComponentScale();
    FMatrix RotA = GetRotationMatrix();

    // 2) 상대 OBB 정보
    FVector CenterB = OtherBox->GetComponentLocation();
    FVector ExtentB = OtherBox->GetBoxExtent() * OtherBox->GetComponentScale();
    FMatrix RotB = OtherBox->GetRotationMatrix();

    // 3) 두 중심 벡터를 A의 로컬 축(x,y,z)으로 투영
    FVector d = CenterB - CenterA;
    FVector Ax(RotA.M[0][0], RotA.M[1][0], RotA.M[2][0]);
    FVector Ay(RotA.M[0][1], RotA.M[1][1], RotA.M[2][1]);
    FVector Az(RotA.M[0][2], RotA.M[1][2], RotA.M[2][2]);
    float T[3] = { d.Dot(Ax), d.Dot(Ay), d.Dot(Az) };

    // 4) 회전 관계 행렬 Rmat과 절댓값 AbsR
    float Rmat[3][3], AbsR[3][3];
    FVector Baxes[3] = {
        FVector(RotB.M[0][0], RotB.M[1][0], RotB.M[2][0]),
        FVector(RotB.M[0][1], RotB.M[1][1], RotB.M[2][1]),
        FVector(RotB.M[0][2], RotB.M[1][2], RotB.M[2][2])
    };
    FVector Aaxes[3] = { Ax, Ay, Az };
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float rij = Aaxes[i].Dot(Baxes[j]);
            Rmat[i][j] = rij;
            AbsR[i][j] = fabs(rij) + 1e-3f;
        }
    }

    // 5) A의 로컬 축(X,Y,Z) 검사
    for (int i = 0; i < 3; ++i)
    {
        float ra = ExtentA[i];
        float rb = ExtentB[0] * AbsR[i][0] + ExtentB[1] * AbsR[i][1] + ExtentB[2] * AbsR[i][2];
        if (fabs(T[i]) > ra + rb) return false;
    }

    // 6) B의 로컬 축(X,Y,Z) 검사
    for (int j = 0; j < 3; ++j)
    {
        float ra = ExtentA[0] * AbsR[0][j] + ExtentA[1] * AbsR[1][j] + ExtentA[2] * AbsR[2][j];
        float rb = ExtentB[j];
        float t = T[0] * Rmat[0][j] + T[1] * Rmat[1][j] + T[2] * Rmat[2][j];
        if (fabs(t) > ra + rb) return false;
    }

    // 7) 교차축(A_i x B_j) 9개 검사
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int i1 = (i + 1) % 3, i2 = (i + 2) % 3;
            int j1 = (j + 1) % 3, j2 = (j + 2) % 3;
            // 투영 반지름 계산
            float ra = ExtentA[i1] * AbsR[i2][j] + ExtentA[i2] * AbsR[i1][j];
            float rb = ExtentB[j1] * AbsR[i][j2] + ExtentB[j2] * AbsR[i][j1];
            // 두 중심 차 벡터의 교차축 투영 거리
            float t = fabs(
                T[i2] * Rmat[i1][j] - T[i1] * Rmat[i2][j]
            );
            if (t > ra + rb) return false;
        }
    }

    return true;
}

bool UBoxShapeComponent::CollisionCheckWithSphere(const USphereShapeComponent* OtherSphere) const
{
    // 1. Sphere 정보
    FVector CenterS = OtherSphere->GetComponentLocation();
    float RadiusS = OtherSphere->GetRadius() * OtherSphere->GetComponentScale().MaxValue();

    // 2. Box OBB 정보
    FVector CenterB = GetComponentLocation();
    FVector ExtentB = GetBoxExtent() * GetComponentScale();
    FMatrix RotB = GetRotationMatrix();

    // 3. Sphere 중심을 Box 로컬 좌표로 변환
    FVector d = CenterS - CenterB;
    FVector AxisBX(RotB.M[0][0], RotB.M[1][0], RotB.M[2][0]);
    FVector AxisBY(RotB.M[0][1], RotB.M[1][1], RotB.M[2][1]);
    FVector AxisBZ(RotB.M[0][2], RotB.M[1][2], RotB.M[2][2]);

    float x = d.Dot(AxisBX);
    float y = d.Dot(AxisBY);
    float z = d.Dot(AxisBZ);

    // 4. Box 내부로의 투영점(Clamp)
    float cx = FMath::Clamp(x, -ExtentB.x, ExtentB.x);
    float cy = FMath::Clamp(y, -ExtentB.y, ExtentB.y);
    float cz = FMath::Clamp(z, -ExtentB.z, ExtentB.z);

    // 5. 최근접 점과 Sphere 중심 거리 계산
    float dx = x - cx;
    float dy = y - cy;
    float dz = z - cz;
    float Dist2 = dx * dx + dy * dy + dz * dz;

    // 6. 반지름 비교: 거리가 Sphere 반지름보다 작으면 충돌
    return Dist2 <= RadiusS * RadiusS;
}

bool UBoxShapeComponent::CollisionCheckWithCapsule(const UCapsuleShapeComponent* OtherCapsule) const
{
    // 1. Capsule 세그먼트 엔드포인트 계산
    FVector CenterC = OtherCapsule->GetComponentLocation();
    float HalfH = OtherCapsule->GetHalfHeight();
    float RadiusC = OtherCapsule->GetRadius() * OtherCapsule->GetComponentScale().MaxValue();
    // 캡슐 로컬 Z축을 기준으로 세그먼트 방향 정의
    FMatrix RotC = OtherCapsule->GetRotationMatrix();
    FVector AxisC = FVector(RotC.M[0][2], RotC.M[1][2], RotC.M[2][2]);
    FVector P0 = CenterC + AxisC * HalfH;
    FVector P1 = CenterC - AxisC * HalfH;

    // 2. 박스 로컬 좌표로 변환
    FVector CenterB = GetComponentLocation();
    FMatrix RotB = GetRotationMatrix();
    FVector AxisBX(RotB.M[0][0], RotB.M[1][0], RotB.M[2][0]);
    FVector AxisBY(RotB.M[0][1], RotB.M[1][1], RotB.M[2][1]);
    FVector AxisBZ(RotB.M[0][2], RotB.M[1][2], RotB.M[2][2]);
    auto ToLocal = [&](const FVector& P) {
        FVector d = P - CenterB;
        return FVector(d.Dot(AxisBX), d.Dot(AxisBY), d.Dot(AxisBZ));
        };
    FVector L0 = ToLocal(P0);
    FVector L1 = ToLocal(P1);

    // 3. 확장된 AABB extents
    FVector ExtentB = GetBoxExtent() * GetComponentScale();
    FVector Exp = ExtentB + FVector(RadiusC, RadiusC, RadiusC);

    // 4. 세그먼트-AABB 교차 (슬랩 방법)
    FVector dLocal = L1 - L0;
    float tmin = 0.0f, tmax = 1.0f;
    for (int i = 0; i < 3; ++i)
    {
        float p0 = (i == 0 ? L0.x : (i == 1 ? L0.y : L0.z));
        float di = (i == 0 ? dLocal.x : (i == 1 ? dLocal.y : dLocal.z));
        float minB = (i == 0 ? -Exp.x : (i == 1 ? -Exp.y : -Exp.z));
        float maxB = (i == 0 ? Exp.x : (i == 1 ? Exp.y : Exp.z));
        if (FMath::Abs(di) < KINDA_SMALL_NUMBER)
        {
            if (p0 < minB || p0 > maxB)
                return false;
        }
        else
        {
            float ood = 1.0f / di;
            float t1 = (minB - p0) * ood;
            float t2 = (maxB - p0) * ood;
            if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
            tmin = FMath::Max(tmin, t1);
            tmax = FMath::Min(tmax, t2);
            if (tmin > tmax)
                return false;
        }
    }
    return true;
}
