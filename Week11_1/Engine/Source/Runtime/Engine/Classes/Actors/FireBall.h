#pragma once
#include "GameFramework/Actor.h"

class UFireBallComponent;
class UBillboardComponent;
class UProjectileMovementComponent;
class UPointLightComponent;
class AFireBall: public AActor
{
    DECLARE_CLASS(AFireBall, AActor)
public:
    AFireBall();
    virtual ~AFireBall() override = default;
    
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    
    void Destroyed() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    bool Destroy() override;
    
    UObject* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    void PostDuplicate() override;
    
    void LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray) override;
    FActorInfo GetActorInfo() override;

private:
    UFireBallComponent* FireBallComponent;
    UPointLightComponent* LightComp;
    UProjectileMovementComponent* ProjMovementComp;
    UBillboardComponent* BillboardComponent;
};
