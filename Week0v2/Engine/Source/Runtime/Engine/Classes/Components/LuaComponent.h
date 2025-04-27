#pragma once
#include <sol/sol.hpp>

#include "ActorComponent.h"
#include "Math/Quat.h"
#include "UObject/ObjectMacros.h"
#include "ActorComponentInfo.h"
#include "Math/Rotator.h"

struct FLuaComponentInfo : public FActorComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FLuaComponentInfo);
    
    FString LuaScriptPath;
    

    FLuaComponentInfo()
        : FActorComponentInfo()
        , LuaScriptPath(TEXT(""))
    {
        InfoType = TEXT("FLuaComponentInfo");
    }
    

    virtual void Serialize(FArchive& ar) const override
    {
        FActorComponentInfo::Serialize(ar);
        ar << LuaScriptPath;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FActorComponentInfo::Deserialize(ar);
        ar >> LuaScriptPath;
    }
};


class ULuaComponent : public UActorComponent
{
    DECLARE_CLASS(ULuaComponent, UActorComponent)
public:

    ULuaComponent();
    ULuaComponent(const ULuaComponent& Other);
    virtual ~ULuaComponent() override;

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
    // 이 컴포넌트가 사용할 Lua 스크립트 파일 경로
    FString LuaScriptPath;

    // AActor의 BeginPlay 대신 컴포넌트의 초기화 함수에서 로드
    virtual void InitializeComponent() override; // 또는 BeginPlay

    // AActor의 Tick 대신 컴포넌트의 Tick 함수
    virtual void TickComponent(float DeltaTime) override;

    /** 컴포넌트가 제거되거나 소유자 Actor가 파괴될 때 호출됩니다. */
    virtual void UninitializeComponent();

    /** 모든 초기화가 끝나고, 준비가 되었을 때 호출됩니다. */
    virtual void BeginPlay();

    /** Component가 제거되었을 때 호출됩니다. */
    virtual void OnComponentDestroyed();
    
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

    /** 이 컴포넌트를 제거합니다. */
    virtual void DestroyComponent();

    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    void LoadAndConstruct(const FActorComponentInfo& Info) override;

    //sol::table LuaData;

private:
    // Lua 함수 포인터들 (AActor 대신 컴포넌트가 소유)
    sol::protected_function LuaFunctionBeginPlay;
    sol::protected_function LuaFunctionTick;
    sol::protected_function LuaOnOverlapFunction;
    // ...

    // 내부적으로 스크립트 테이블을 잠시 저장할 수도 있지만,
    // 주로 필요한 함수 포인터를 추출하여 멤버로 가지는 것이 일반적입니다.
};
