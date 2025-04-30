#pragma once
#include <sol/sol.hpp>

#include "ActorComponent.h"
#include "Math/Quat.h"
#include "UObject/ObjectMacros.h"
#include "ActorComponentInfo.h"
#include "GameFramework/Actor.h"
#include "Math/Rotator.h"
#include "Script/LuaManager.h"

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
    //void ExecuteLuaFunction(sol::protected_function LuaFunction);

    void CallLuaFunction(const FString& FunctionName, const TArray<sol::object>& Args = TArray<sol::object>());

    sol::table ActorLuaData;
    sol::table ScriptTable;
private:
    // Lua 함수 포인터들 (AActor 대신 컴포넌트가 소유)
    sol::protected_function LuaFunctionBeginPlay;
    sol::protected_function LuaFunctionTick;
    sol::protected_function LuaFunctionOnOverlap;
    sol::protected_function LuaFunctionEndPlay;
    sol::protected_function LuaFunctionDestroyComponent;
    // ...

        /**
     * @brief 주어진 Lua 함수를 액터 및 액터의 Lua 데이터 컨텍스트 내에서 안전하게 실행합니다.
     *        임의의 개수의 인자를 Lua 함수로 전달할 수 있습니다.
     *
     * 함수 실행 전에 Lua 전역 환경에 'Actor'와 'ActorLuaData'를 설정하고,
     * 실행 후에는 초기화합니다.
     *
     * @tparam Args Lua 함수에 전달될 인자들의 타입들.
     * @param LuaFunction 실행할 보호된 Lua 함수 객체.
     * @param args Lua 함수에 전달할 가변 인자들.
     */
    template <typename... Args> // 가변 인자 템플릿 사용
    sol::protected_function_result ExecuteLuaFunction(sol::protected_function LuaFunction, Args&&... args) // 인자를 완벽 전달(forwarding)
    {
        // 함수 유효성 검사
        if (!LuaFunction.valid()) {
            // 유효하지 않은 결과를 반환하거나, 특정 오류 코드를 반환할 수 있음
             return sol::protected_function_result(); // 기본 생성 (보통 실패 상태)
        }

        AActor* OwnerActor = GetOwner();


        FLuaManager& LuaMan = FLuaManager::Get();
        sol::state& lua = LuaMan.GetLuaState();

        // --- 컨텍스트 설정 ---
        sol::object previousActor = lua["Actor"];
        sol::object previousActorLuaData = lua["ActorLuaData"];
        lua["Actor"] = OwnerActor;
        lua["ActorLuaData"] = ActorLuaData;

        // --- Lua 함수 실행 (가변 인자 전달!) ---
        // std::forward를 사용하여 인자들을 Lua 함수 호출로 완벽하게 전달합니다.
        // Sol2는 이 인자들을 Lua 타입으로 변환하여 함수를 호출합니다.
        sol::protected_function_result result = LuaFunction(std::forward<Args>(args)...);

        // --- 컨텍스트 복원/정리 ---
        lua["Actor"] = previousActor;
        lua["ActorLuaData"] = previousActorLuaData;

        // --- 결과 확인 및 로깅 (선택 사항) ---
        if (!result.valid()) {
            sol::error err = result;
            UE_LOG(LogLevel::Error, TEXT("Error executing Lua function for Actor %s: %hs"),
                   *OwnerActor->GetName(), // GetName() 사용 권장
                   err.what());
        }

        // 실행 결과를 반환
        return result;
    }

    // --- 새로 추가할 함수 ---
    /**
     * @brief ScriptTable 내의 함수를 이름으로 찾아 컨텍스트 내에서 실행합니다.
     *
     * ScriptTable에서 FunctionName에 해당하는 함수를 찾아 ExecuteLuaFunction을 호출합니다.
     * 임의의 개수의 인자를 Lua 함수로 전달할 수 있습니다.
     *
     * @tparam Args Lua 함수에 전달될 인자들의 타입들.
     * @param FunctionName ScriptTable 내에서 찾을 함수의 이름 (문자열).
     * @param args Lua 함수에 전달할 가변 인자들.
     * @return Lua 함수의 실행 결과. 함수를 찾지 못하거나 유효하지 않으면 실패 상태 반환.
     */
    template <typename... Args>
    sol::protected_function_result ExecuteFunctionByName(const std::string& FunctionName, Args&&... args)
    {
        // 1. ScriptTable 유효성 확인
        if (!ScriptTable.valid()) {
            UE_LOG(LogLevel::Error, TEXT("ExecuteFunctionByName failed: ScriptTable is invalid for Actor %s."),
                   *GetOwner()->GetName());
            return sol::protected_function_result(); // 실패 반환
        }

        // 2. ScriptTable에서 이름으로 함수 객체 가져오기
        sol::object luaFuncObj = ScriptTable[FunctionName];

        // 3. 가져온 객체가 유효한 함수인지 확인
        if (!luaFuncObj.valid() || !luaFuncObj.is<sol::protected_function>()) {
            // 함수가 없거나 함수 타입이 아닌 경우
            UE_LOG(LogLevel::Error, TEXT("ExecuteFunctionByName: Function '%hs' not found or not a function in ScriptTable for Actor %s."),
                   FunctionName.c_str(),
                   *GetOwner()->GetName());
            return sol::protected_function_result(); // 실패 반환
        }

        // 4. 유효한 함수 객체로 변환
        sol::protected_function luaFunc = luaFuncObj.as<sol::protected_function>();

        // 5. 기존 ExecuteLuaFunction 함수를 호출하여 실행 위임
        //    컨텍스트 설정, 인자 전달, 실행, 컨텍스트 복원은 ExecuteLuaFunction이 처리
        return ExecuteLuaFunction(luaFunc, std::forward<Args>(args)...);
    }

};



