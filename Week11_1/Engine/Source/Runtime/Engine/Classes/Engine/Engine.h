#pragma once
#include "EngineBaseTypes.h"
#include "EngineTypes.h"
#include "World.h"
#include "Container/Array.h"
#include "UObject/ObjectMacros.h"


class UAssetImporter;
class UAssetManager;
struct FWorldContext
{
public:
    FWorldContext()
        : WorldType(EWorldType::None)
        , LevelType(LEVELTICK_All)
        , World(nullptr)
    {
    }

    EWorldType::Type WorldType;
    // TArray<FWorldContext*> ExternalReferences;
    ELevelTick LevelType;

    UWorld* GetWorld() const { return World; }
    void SetWorld(UWorld* InWorld) { World = InWorld; }

private:
    UWorld* World;
};

class UEngine : public UObject
{
    DECLARE_ABSTRACT_CLASS(UEngine, UObject)

public:
    UEngine() = default;
    ~UEngine() override = default;

public:
    virtual void Init();
    virtual void Tick(float DeltaTime);
    virtual void Release() {}

    UAssetManager* AssetManager = nullptr;
    UAssetImporter* AssetImporter = nullptr;

    static inline UINT GFrameCount = 0;

    //std::shared_ptr<FWorldContext> GetWorldContextByKey(FName Key);

protected:
    TMap<uint32, std::shared_ptr<FWorldContext>> WorldContexts;
};
