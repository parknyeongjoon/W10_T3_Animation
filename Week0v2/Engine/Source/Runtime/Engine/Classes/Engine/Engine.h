#pragma once
#include "EngineTypes.h"
#include "Container/Array.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"


class UWorld;
struct FWorldContext
{
    FWorldContext() : WorldType(EWorldType::None), thisCurrentWorld(nullptr){}

    UWorld* thisCurrentWorld;
    EWorldType::Type WorldType;
    TArray<FWorldContext*> ExternalReferences;

    UWorld* World() { return thisCurrentWorld; }
};

class UEngine : public UObject
{
    DECLARE_CLASS(UEngine, UObject)

public:
    UEngine();
    ~UEngine();
    virtual int32 Init(HWND hwnd);
    virtual void Tick(float deltaSceconds);

protected:
    TArray<FWorldContext> worldContexts;
    FWorldContext* GetEditorWorldContext() { return &worldContexts[0]; }

public:
    TArray<FWorldContext>& GetWorldContexts() { return worldContexts; }
    
};
