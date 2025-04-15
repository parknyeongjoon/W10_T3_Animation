#pragma once
#include "Components/FireBallComponent.h"
#include "GameFramework/Actor.h"

class UBillboardComponent;
class UProjectileMovementComponent;
class UPointLightComponent;
class AFireBallActor: public AActor
{
    DECLARE_CLASS(AFireBallActor, AActor)
public:
    AFireBallActor();
    AFireBallActor(const AFireBallActor& Other);
    virtual ~AFireBallActor() override = default;
    
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    
    void Destroyed() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    bool Destroy() override;
    
    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
    
    void LoadAndConstruct(const TArray<std::shared_ptr<FActorComponentInfo>>& InfoArray) override;
    FActorInfo GetActorInfo() override;

private:
    UFireBallComponent* FireBallComponent;
    UPointLightComponent* LightComp;
    UProjectileMovementComponent* ProjMovementComp;
    UBillboardComponent* BillboardComponent;
};
