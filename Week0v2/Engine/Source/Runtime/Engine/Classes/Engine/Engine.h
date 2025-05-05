#pragma once
#include "EngineTypes.h"
#include "Container/Array.h"
#include "UObject/ObjectMacros.h"


class UAssetManager;
struct FWorldContext
{
public:
    FWorldContext() : WorldType(EWorldType::None), World(nullptr){}

    EWorldType::Type WorldType;
    // TArray<FWorldContext*> ExternalReferences;

    UWorld* GetWorld() { return World; }
    void SetWorld(UWorld* InWorld) { World = InWorld; }
    
private:
    UWorld* World;
};

class UEngine : public UObject
{
    DECLARE_CLASS(UEngine, UObject)

public:
    UEngine() = default;
    virtual ~UEngine() = default;

public:
    virtual void Init();
    virtual void Tick(float DeltaTime);
    virtual void Release() {}
    UWorld* GetWorld() override;

    UAssetManager* AssetManager = nullptr;
    
    TArray<std::shared_ptr<FWorldContext>>& GetWorldContexts() { return WorldContexts; }

    static inline UINT GFrameCount = 0;

protected:
    TArray<std::shared_ptr<FWorldContext>> WorldContexts;
};
