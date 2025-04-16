#pragma once
#include "GameFramework/Actor.h"
#include <Components/HeightFogComponent.h>

class AExponentialHeightFog : public AActor
{
    DECLARE_CLASS(AExponentialHeightFog, AActor)
public:
    AExponentialHeightFog();
    AExponentialHeightFog(const AExponentialHeightFog& Other);
    virtual ~AExponentialHeightFog() override;
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

protected:
    UHeightFogComponent* FogComponent = nullptr;
};