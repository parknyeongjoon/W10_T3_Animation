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
    TArray<FWorldContext*> ExternalReferences;

    UWorld* GetWorld() { return World; }
    void SetWorld(UWorld* InWorld) { World = InWorld; }
    
private:
    UWorld* World;
};

class UEngine : public UObject
{
    DECLARE_CLASS(UEngine, UObject)

public:
    UEngine();
    virtual ~UEngine();

public:
    virtual void Init(HWND hWnd);
    virtual void Tick(float DeltaTime);
    virtual void Release() {}
    UWorld* GetWorld() override;

    UAssetManager* AssetManager = nullptr;
    
    TArray<FWorldContext>& GetWorldContexts() { return worldContexts; }

    static inline UINT GFrameCount = 0;

protected:
    TArray<FWorldContext*> WorldContexts;
};
