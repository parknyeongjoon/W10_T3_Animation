#pragma once
#include "Classes/GameFramework/Actor.h"

class UBillboardComponent;
class ULightComponentBase;

class ALight : public AActor
{
    DECLARE_CLASS(ALight, AActor)
public:
    ALight();
    virtual ~ALight() override;
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

protected:
    ULightComponentBase* LightComponent;
    UBillboardComponent* BillboardComponent;
};

