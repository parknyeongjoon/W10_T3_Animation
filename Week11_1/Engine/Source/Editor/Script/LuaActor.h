#pragma once
#include "GameFramework/Actor.h"


class ALuaActor : public AActor
{
    DECLARE_CLASS(ALuaActor, AActor)

public:
    ALuaActor() = default;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    class ULuaComponent* GetLuaComponent() const    {
        return LuaComponent;
    }

private:
    ULuaComponent* LuaComponent = nullptr;
};
