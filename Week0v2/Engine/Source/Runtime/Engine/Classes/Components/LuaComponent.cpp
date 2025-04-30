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

    ExecuteLuaFunction(LuaFunctionTick, DeltaTime);;

}

void ULuaComponent::UninitializeComponent()
{
    Super::UninitializeComponent();
}

void ULuaComponent::BeginPlay()
{
    Super::BeginPlay();

    FLuaManager& LuaMan = FLuaManager::Get();
        

        ScriptTable = LuaMan.GetOrLoadScriptTable(LuaScriptPath);
        if (ScriptTable.valid())
        {
            // 함수 추출
            LuaFunctionBeginPlay = ScriptTable["BeginPlay"];
            LuaFunctionTick = ScriptTable["Tick"];
            LuaFunctionOnOverlap = ScriptTable["OnOverlap"];
            LuaFunctionEndPlay = ScriptTable["EndPlay"];
            LuaFunctionDestroyComponent = ScriptTable["DestroyComponent"];
            
            // ...

            // // 컴포넌트의 LuaData 초기화
            // if (!LuaData.valid()) {
            //     LuaData = lua.create_table();
            // }

            // LuaData 테이블 초기화 (여기서 해주는 것이 좋음)
            if (!ActorLuaData.valid()) {
                sol::state& lua = FLuaManager::Get().GetLuaState();
                ActorLuaData = lua.create_table(); // 빈 테이블로 초기화
            }
            
            ExecuteLuaFunction(LuaFunctionBeginPlay);
        } else {
            // 스크립트 로드 실패 처리
        }
}

void ULuaComponent::OnComponentDestroyed()
{
    Super::OnComponentDestroyed();
    ExecuteLuaFunction(LuaFunctionOnOverlap);
    
}

void ULuaComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    ExecuteLuaFunction(LuaFunctionEndPlay);
}

void ULuaComponent::DestroyComponent()
{
    Super::DestroyComponent();
    
    ExecuteLuaFunction(LuaFunctionDestroyComponent);
}

std::unique_ptr<FActorComponentInfo> ULuaComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FLuaComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void ULuaComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FLuaComponentInfo* Info = static_cast<FLuaComponentInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);
    
    Info->LuaScriptPath = LuaScriptPath;
}

void ULuaComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);

    // cast
    const FLuaComponentInfo& TextInfo = static_cast<const FLuaComponentInfo&>(Info);
    LuaScriptPath = TextInfo.LuaScriptPath;
}



void ULuaComponent::CallLuaFunction(const FString& FunctionName, const TArray<sol::object>& Args)
{
    sol::protected_function LuaFunction = ScriptTable[FunctionName];
    ExecuteLuaFunction(LuaFunction);
}