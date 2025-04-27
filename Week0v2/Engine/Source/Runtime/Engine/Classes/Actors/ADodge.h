#pragma once
#include "Engine/StaticMeshActor.h"
#include "Core/Delegates/DelegateCombination.h"

class UStaticMeshComponent;

class ADodge : public AStaticMeshActor
{
    DECLARE_CLASS(ADodge, AStaticMeshActor)
    DECLARE_MULTICAST_DELEGATE(TEST_DELIGATE)
public:
    ADodge();
    ADodge(const ADodge& Other);
    virtual ~ADodge() override = default;
    
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    
    void Destroyed() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    bool Destroy() override;
    void test();

    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
    
    void LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray) override;
    FActorInfo GetActorInfo() override;

private:
    TEST_DELIGATE TestDelegate;
};
