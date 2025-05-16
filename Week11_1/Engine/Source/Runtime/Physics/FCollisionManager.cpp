#include "FCollisionManager.h"

#include "Components/PrimitiveComponents/Physics/UShapeComponent.h" 
#include "Components/PrimitiveComponents/PrimitiveComponent.h"
#include "GameFramework/ACtor.h"
#include "UserInterface/Console.h"

void FCollisionManager::Initialize()
{
    RegisteredComponents.Empty();
    PreviousOverlaps.Empty();
    CurrentOverlaps.Empty();
    PendingHitResults.Empty();
    PendingBeginOverlapEvents.Empty();
    PendingEndOverlapEvents.Empty();

    UE_LOG(LogLevel::Display, "Collision Manager InitialIzed");
}

void FCollisionManager::Release()
{
    RegisteredComponents.Empty();
    PreviousOverlaps.Empty();
    CurrentOverlaps.Empty();
    PendingHitResults.Empty();
    PendingBeginOverlapEvents.Empty();
    PendingEndOverlapEvents.Empty();
}

void FCollisionManager::Register(const UPrimitiveComponent* InComponent)
{
    if (InComponent && !RegisteredComponents.Contains(InComponent))
    {
        RegisteredComponents.Add(InComponent);
        //UE_LOG(LogLevel::Display, "Register : %s", *(InComponent->GetOwner()->GetActorLabel()));
        UE_LOG(LogLevel::Display, "Registered Count : %d", RegisteredComponents.Num());
    }
}

void FCollisionManager::Unregister(const UPrimitiveComponent* InComponent)
{
    if (InComponent && RegisteredComponents.Contains(InComponent))
    {
        RegisteredComponents.Remove(InComponent);
    }
}

void FCollisionManager::UpdateCollision(float DeltaTime)
{    
    // Broad Phase Collision Check
    BroadPhaseCollisionCheck(DeltaTime);

    // Narrow Phase Collision Check
    NarrowPhaseCollisionCheck(DeltaTime);

    // Sweep Collision Check
    //for (int i = 0; i < RegisteredComponents.Num(); i++)
    //{
    //    for (int j = i + 1; j < RegisteredComponents.Num(); j++)
    //    {
    //        HitResult HitResult;
    //        SweepCollisionCheck(RegisteredComponents[i], RegisteredComponents[j], HitResult);
    //        if (HitResult.HitComponent)
    //        {
    //            PendingHitResults.Add(HitResult);
    //        }
    //    }
    //}

    //// Notify Hit Events
    //NotifyHitEvents();

    NotifyBeginOverlapEvents();
    NotifyEndOverlapEvents();
}

void FCollisionManager::NotifyHitEvents()
{
    for (const HitResult& Hit : PendingHitResults)
    {
        if (Hit.HitComponent)
        {
            // Notify the hit event to the component
            Hit.HitComponent->NotifyHit(Hit);
        }
    }
    PendingHitResults.Empty();
}

void FCollisionManager::NotifyBeginOverlapEvents()
{
    for (const FComponentPair& Pair : PendingBeginOverlapEvents)
    {
        if (Pair.Key && Pair.Value)
        {
            Pair.Key->NotifyBeginOverlap(Pair.Value); // TODO : OverlapResult로 변경
            Pair.Value->NotifyBeginOverlap(Pair.Key); // TODO : OverlapResult로 변경
        }
    }
    PendingBeginOverlapEvents.Empty();

}

void FCollisionManager::NotifyEndOverlapEvents()
{
    for (const FComponentPair& Pair : PendingEndOverlapEvents)
    {
        if (Pair.Key && Pair.Value)
        {
            Pair.Key->NotifyEndOverlap(Pair.Value); // TODO : OverlapResult로 변경
            Pair.Value->NotifyEndOverlap(Pair.Key); // TODO : OverlapResult로 변경
        }
    }
    PendingEndOverlapEvents.Empty();
}

void FCollisionManager::BroadPhaseCollisionCheck(float DeltaTime)
{
    BroadPhaseOverlapPairs.Empty();

    int32 Num = RegisteredComponents.Num();
    for (size_t i = 0; i < Num; i++)
    {
        for (size_t j = i + 1; j < Num; j++)
        {
            const UPrimitiveComponent* CompA = RegisteredComponents[i];
            const UPrimitiveComponent* CompB = RegisteredComponents[j];

            // 충돌 채널 필터링
            if (!ShouldCollide(CompA, CompB))
                continue;

            // ShapeComponent 끼리만 검사
            const UShapeComponent* ShapeA = dynamic_cast<const UShapeComponent*>(CompA);
            const UShapeComponent* ShapeB = dynamic_cast<const UShapeComponent*>(CompB);
            if (ShapeA && ShapeB)
            {
                if (ShapeA->BroadPhaseCollisionCheck(ShapeB))
                {
                    BroadPhaseOverlapPairs.Add(FComponentPair(ShapeA, ShapeB));
                }
            }
        }
    }
}

void FCollisionManager::NarrowPhaseCollisionCheck(float DeltaTime)
{
    CurrentOverlaps.Empty();

    // BroadPhaseOverlapPairs를 확인해 Overlap 검사
    for (const FComponentPair& Pair : BroadPhaseOverlapPairs)
    {
        const UShapeComponent* ShapeA = dynamic_cast<const UShapeComponent*>(Pair.Key);
        const UShapeComponent* ShapeB = dynamic_cast<const UShapeComponent*>(Pair.Value);

        if (ShapeA->NarrowPhaseCollisionCheck(ShapeB))
        {
            CurrentOverlaps.Add(Pair);
        }
    }

    // BeginOverlap (Previous : Unoverlap, Current : Overlap)
    for (const FComponentPair& Pair : CurrentOverlaps)
    {
        // 겹침 쌍이 정확히 같아야 함.
        if (!PreviousOverlaps.Contains(Pair))
        {
            PendingBeginOverlapEvents.Add(Pair);
        }
    }
    
    // EndOverlap (Previous : Overlap, Current : Unoverlap)
    for (const FComponentPair& Pair : PreviousOverlaps)
    {
        if (!CurrentOverlaps.Contains(Pair))
        {
            PendingEndOverlapEvents.Add(Pair);
        }
    }

    // 갱신
    PreviousOverlaps = CurrentOverlaps;
}

void FCollisionManager::SweepCollisionCheck(const UPrimitiveComponent* ComponentA, const UPrimitiveComponent* ComponentB, HitResult& OutHitResult)
{

}

bool FCollisionManager::ShouldCollide(const UPrimitiveComponent* ComponentA, const UPrimitiveComponent* ComponentB) const
{
    return true;
}
