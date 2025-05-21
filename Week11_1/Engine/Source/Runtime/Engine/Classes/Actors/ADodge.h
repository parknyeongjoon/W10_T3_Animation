#pragma once
#include "Engine/StaticMeshActor.h"
#include "Core/Delegates/DelegateCombination.h"
#include "UObject/FunctionRegistry.h"

class UStaticMeshComponent;

class ADodge : public AStaticMeshActor
{
    DECLARE_CLASS(ADodge, AStaticMeshActor)
    DECLARE_MULTICAST_DELEGATE(TEST_DELIGATE)
public:
    ADodge();
    virtual ~ADodge() override = default;
    
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    
    void Destroyed() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    bool Destroy() override;
    void TestTranslate();
    void TestRotate();

    UObject* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    void PostDuplicate() override;
    
    void LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray) override;
    FActorInfo GetActorInfo() override;

    TEST_DELIGATE TestDelegate;
};
