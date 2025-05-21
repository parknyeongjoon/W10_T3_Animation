#pragma once
#include "Core/Math/Vector.h"
#include "Core/Container/Array.h"
#include "Core/Container/Pair.h"

class UPrimitiveComponent;

struct HitResult
{
    const UPrimitiveComponent* HitComponent = nullptr;
    const UPrimitiveComponent* OtherComponent = nullptr;
    FVector HitLocation;
    FVector HitNormal;
    FVector HitPoint;
    float HitTime = 0.0f;
};

struct OverlapResult
{
    const UPrimitiveComponent* OverlappedComponent = nullptr;
    const UPrimitiveComponent* OtherComponent = nullptr;
    FVector OverlapLocation;
    FVector OverlapNormal;
    float OverlapTime = 0.0f;
};

using FComponentPair = TPair<const UPrimitiveComponent*, const UPrimitiveComponent*>;

class FCollisionManager
{
public:
    FCollisionManager() = default;
    ~FCollisionManager() = default;

    void Initialize();
    void Release();

    // 충돌 체크 (매프레임 호출)
    void UpdateCollision(float DeltaTime);

    // 충돌 대상 등록, 해제
    void Register(const UPrimitiveComponent* InComponent);
    void Unregister(const UPrimitiveComponent* InComponent);

private:
    // Broad Phase -> Narrow Phase -> Sweep Check
    void BroadPhaseCollisionCheck(float DeltaTime);
    void NarrowPhaseCollisionCheck(float DeltaTime);
    void SweepCollisionCheck(const UPrimitiveComponent* ComponentA 
                            ,const UPrimitiveComponent* ComponentB
                            ,HitResult& OutHitResult);

    // 충돌 필터링 (채널)
    bool ShouldCollide(const UPrimitiveComponent* ComponentA, const UPrimitiveComponent* ComponentB) const;

    // TODO: BVH, Uniform Grid, Octree 등 공간 분할 가속 구조 적용

    // 이벤트 판정 후 디스패치
    void NotifyHitEvents();
    void NotifyBeginOverlapEvents();
    void NotifyEndOverlapEvents();

private:
    // 등록된 컴포넌트
    TArray<const UPrimitiveComponent*> RegisteredComponents;

    // Overlap 이벤트 판정을 위한 이전/현재 프레임 상태
    TArray<FComponentPair> PreviousOverlaps;
    TArray<FComponentPair> CurrentOverlaps;

    // 알림 대기 중인 이벤트 저장
    TArray<HitResult> PendingHitResults;
    TArray<FComponentPair> PendingBeginOverlapEvents;
    TArray<FComponentPair> PendingEndOverlapEvents;

    TArray<FComponentPair> BroadPhaseOverlapPairs;
};

