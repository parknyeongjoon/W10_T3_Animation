#include "FCollisionManager.h"
#include "Classes/Components/PrimitiveComponents/PrimitiveComponent.h"

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
        UE_LOG(LogLevel::Display, "Register : %s", InComponent->GetName());
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
    for (int i = 0; i < RegisteredComponents.Num(); i++)
    {
        for (int j = i + 1; j < RegisteredComponents.Num(); j++)
        {
            HitResult HitResult;
            SweepCollisionCheck(RegisteredComponents[i], RegisteredComponents[j], HitResult);
            if (HitResult.HitComponent)
            {
                PendingHitResults.Add(HitResult);
            }
        }
    }
    // Notify Hit Events
    NotifyHitEvents();
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

}

void FCollisionManager::NotifyEndOverlapEvents()
{
}

void FCollisionManager::BroadPhaseCollisionCheck(float DeltaTime)
{
}

void FCollisionManager::NarrowPhaseCollisionCheck(float DeltaTime)
{
}

void FCollisionManager::SweepCollisionCheck(const UPrimitiveComponent* ComponentA, const UPrimitiveComponent* ComponentB, HitResult& OutHitResult)
{
}

bool FCollisionManager::ShouldCollide(const UPrimitiveComponent* ComponentA, const UPrimitiveComponent* ComponentB) const
{
    return false;
}
