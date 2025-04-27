#include "LuaComponent.h"

#include "Script/LuaManager.h"
#include "Script/LuaActor.h"

ULuaComponent::ULuaComponent()
    : LuaScriptPath(TEXT(""))
{
}

ULuaComponent::ULuaComponent(const ULuaComponent& Other)
    : Super(Other),
    LuaScriptPath(Other.LuaScriptPath)
{
}

ULuaComponent::~ULuaComponent()
{
}

UObject* ULuaComponent::Duplicate() const
{
    ULuaComponent* NewComp = FObjectFactory::ConstructObjectFrom<ULuaComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void ULuaComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void ULuaComponent::PostDuplicate()
{
    Super::PostDuplicate();
}

void ULuaComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void ULuaComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    if (LuaFunctionTick.valid())
    {
        sol::state& lua = FLuaManager::Get().GetLuaState();
        lua["Actor"] = GetOwner(); // 컨텍스트 설정
        //lua["comp"] = this;     // 컨텍스트 설정
        sol::protected_function_result result = LuaFunctionTick(DeltaTime);
        // 오류 처리...
        lua["Actor"] = sol::lua_nil;
        //lua["comp"] = sol::lua_nil; // 컨텍스트 해제
    }
}

void ULuaComponent::UninitializeComponent()
{
    Super::UninitializeComponent();
}

void ULuaComponent::BeginPlay()
{
    Super::BeginPlay();

    FLuaManager& LuaMan = FLuaManager::Get();
        sol::state& lua = LuaMan.GetLuaState();

        sol::table ScriptTable = LuaMan.GetOrLoadScriptTable(LuaScriptPath);
        if (ScriptTable.valid())
        {
            // 함수 추출
            LuaFunctionBeginPlay = ScriptTable["BeginPlay"];
            LuaFunctionTick = ScriptTable["Tick"];
            LuaOnOverlapFunction = ScriptTable["OnOverlap"];
            // ...

            // // 컴포넌트의 LuaData 초기화
            // if (!LuaData.valid()) {
            //     LuaData = lua.create_table();
            // }

            // Lua의 BeginPlay 호출 (컨텍스트 설정 주의!)
            if (LuaFunctionBeginPlay.valid()) {
                lua["Actor"] = GetOwner(); 
                }

                sol::protected_function_result result = LuaFunctionBeginPlay();
                lua["Actor"] = sol::lua_nil;
        } else {
            // 스크립트 로드 실패 처리
        }
}

void ULuaComponent::OnComponentDestroyed()
{
    Super::OnComponentDestroyed();
}

void ULuaComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void ULuaComponent::DestroyComponent()
{
    Super::DestroyComponent();
}
