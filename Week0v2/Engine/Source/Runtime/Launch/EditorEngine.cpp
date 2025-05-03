#include "EditorEngine.h"

#include "LaunchEngineLoop.h"
#include "Engine/World.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/UnrealEd.h"
#include "LevelEditor/SLevelEditor.h"
#include "UObject/UObjectIterator.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Contents/UI/ContentsUI.h"
#include "Coroutine/LuaCoroutine.h"
#include "GameFramework/Actor.h"
#include "Physics/FCollisionManager.h"
#include "Script/LuaManager.h"
#include "UnrealEd/EditorPlayer.h"
#include "UObject/Casts.h"
#include "Engine/AssetManager.h"

class ULevel;


FCollisionManager UEditorEngine::CollisionManager;
FCoroutineManager UEditorEngine::CoroutineManager;

UEditorEngine::UEditorEngine()
    : LevelEditor(nullptr)
    , UnrealEditor(nullptr)
    , ActiveWorld(nullptr)
    , ContentsUI(nullptr)
{
}

void UEditorEngine::Init(HWND hWnd)
{
    Super::Init(hWnd);
    LevelEditor = new SLevelEditor();
    UnrealEditor = new UnrealEd();
    ContentsUI = new FContentsUI();

    RECT ClientRect = {};
    GetClientRect(GEngineLoop.AppWnd, &ClientRect);
    float ClientWidth = ClientRect.right - ClientRect.left;
    float ClientHeight = ClientRect.bottom - ClientRect.top;

    /* must be initialized before window. */
    LevelEditor->Initialize(ClientWidth, ClientHeight);
    UnrealEditor->Initialize(LevelEditor);
    ContentsUI->Initialize();
    CollisionManager.Initialize();
    FLuaManager::Get().Initialize();
    
    UnrealEditor->OnResize(hWnd);
    ContentsUI->OnResize(hWnd);

    std::shared_ptr<FWorldContext> EditorContext = CreateNewWorldContext(EWorldType::Editor);

    EditorWorld = FObjectFactory::ConstructObject<UWorld>();
    EditorWorld->WorldType = EWorldType::Editor;
    
    EditorContext->SetWorld(EditorWorld);
    ActiveWorld = EditorWorld;
    ActiveWorld->InitWorld(); // UISOO Check

    EditorPlayer = FObjectFactory::ConstructObject<UEditorPlayer>();
    EditorPlayer->Initialize();
    
    RegisterWaitHelpers(FLuaManager::Get().GetLuaState());

    if (AssetManager == nullptr)
    {
        AssetManager = FObjectFactory::ConstructObject<UAssetManager>();
        assert(AssetManager);
        AssetManager->InitAssetManager();
    }
}

void UEditorEngine::Tick(float deltaSeconds)
{
    ActiveWorld->Tick(LevelType, deltaSeconds);
    EditorPlayer->Tick();
    
    CollisionManager.UpdateCollision(deltaSeconds);
    
    Input();
    
    LevelEditor->Tick(LevelType, deltaSeconds);

    if (LevelEditor->GetEditorStateManager().GetEditorState() != EEditorState::Playing || bForceEditorUI == true )
    {
        UnrealEditor->Render();
    }
    else
    {
        ContentsUI->Render();
    }

    CoroutineManager.Tick(deltaSeconds);
    CoroutineManager.CleanupCoroutines();
}

void UEditorEngine::Release()
{
    ActiveWorld->Release();
    LevelEditor->Release();
    
    CollisionManager.Release();

    FLuaManager::Get().Shutdown();
    
    delete LevelEditor;
    delete UnrealEditor;
    delete ContentsUI;
}

void UEditorEngine::Input()
{
    if (GetAsyncKeyState('L') & 0x8000 or GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        LevelEditor->GetEditorStateManager().SetState(EEditorState::Stopped);
    }


    if (GetAsyncKeyState('P') & 0x8000 and GetAsyncKeyState(VK_MENU) & 0x8000)
    {
        if (LevelEditor->GetEditorStateManager().GetEditorState() == EEditorState::Editing)
        {
            LevelEditor->GetEditorStateManager().SetState(EEditorState::PreparingPlay);
        }
        // else if (LevelEditor->GetEditorStateManager().GetEditorState() == EEditorState::Paused)
        // {
        //     LevelEditor->GetEditorStateManager().SetState(EEditorState::Playing);
        // }
        // else
        // {
        //     LevelEditor->GetEditorStateManager().SetState(EEditorState::Paused);
        // }
    }

    if (GetAsyncKeyState('U') & 0x8000)
    {
        if (!bUButtonDown)
        {
            bUButtonDown= true;
            ForceEditorUIOnOff();
        }
    }
    else
    {
        if (bUButtonDown)
        {
            bUButtonDown = false;
        }
    }
}

UWorld* UEditorEngine::GetWorld()
{
    return ActiveWorld;
}

void UEditorEngine::PreparePIE()
{
    std::shared_ptr<FWorldContext> PIEWorldContext = CreateNewWorldContext(EWorldType::PIE);
    
    PIEWorld = Cast<UWorld>(EditorWorld->Duplicate());
    PIEWorld->WorldType = EWorldType::PIE;

    PIEWorldContext->SetWorld(PIEWorld);
    ActiveWorld = PIEWorld;
    
    LevelType = LEVELTICK_All;    
}

void UEditorEngine::StartPIE()
{
    // 1. BeingPlay() 호출
    ActiveWorld->BeginPlay();
    LevelType = LEVELTICK_All;
    UE_LOG(LogLevel::Error, "Start PIE");
}

void UEditorEngine::PausedPIE()
{
    if (LevelType == LEVELTICK_All)
    {
        LevelType = LEVELTICK_PauseTick;
    }
    else if (LevelType == LEVELTICK_PauseTick)
    {
        LevelType = LEVELTICK_All;
    }
    UE_LOG(LogLevel::Error, "Pause PIE");
}

void UEditorEngine::ResumingPIE()
{

}

void UEditorEngine::StopPIE()
{
    if (PIEWorld == nullptr)
    {
        MessageBox(nullptr, L"PIE WORLD is not exist", nullptr, MB_OK | MB_ICONERROR);
    }

    ActiveWorld->Release();
    
    PIEWorld = nullptr;
    WorldContexts.Remove(WorldContexts[1]);
    
    ActiveWorld = EditorWorld;
    LevelType = LEVELTICK_ViewportsOnly;
}

void UEditorEngine::UpdateGizmos()
{
    for (UGizmoBaseComponent* GizmoComp : TObjectRange<UGizmoBaseComponent>())
    {
        if (ActiveWorld != GizmoComp->GetWorld())
        {
            continue;
        }
        
        if (!ActiveWorld->GetSelectedActors().IsEmpty())
        {
            AActor* PickedActor = *ActiveWorld->GetSelectedActors().begin();
            if (PickedActor == nullptr)
                break;
            std::shared_ptr<FEditorViewportClient> activeViewport = GetLevelEditor()->GetActiveViewportClient();
            if (activeViewport->IsPerspective())
            {
                float scalar = abs(
                    (activeViewport->ViewTransformPerspective.GetLocation() - PickedActor->GetRootComponent()->GetRelativeLocation()).Magnitude()
                );
                scalar *= 0.1f;
                GizmoComp->SetRelativeScale(FVector(scalar, scalar, scalar));
            }
            else
            {
                float scalar = activeViewport->OrthoSize * 0.1f;
                GizmoComp->SetRelativeScale(FVector(scalar, scalar, scalar));
            }
        }
    }
}

std::shared_ptr<FWorldContext> UEditorEngine::CreateNewWorldContext(EWorldType::Type InWorldType)
{
    std::shared_ptr<FWorldContext> NewWorldContext = std::make_shared<FWorldContext>();
    NewWorldContext->WorldType = InWorldType;
    WorldContexts.Add(NewWorldContext);
    

    return NewWorldContext;
}
