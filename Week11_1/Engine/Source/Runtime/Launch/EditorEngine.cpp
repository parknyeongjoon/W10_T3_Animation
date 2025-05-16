#include "EditorEngine.h"

#include "LaunchEngineLoop.h"
#include "PlayerCameraManager.h"
#include "WindowsCursor.h"
#include "Actors/SkeletalMeshActor.h"
#include "Engine/World.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/UnrealEd.h"
#include "LevelEditor/SLevelEditor.h"
#include "UObject/UObjectIterator.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Camera/CameraFadeInOut.h"
#include "Contents/GameManager.h"
#include "Contents/UI/ContentsUI.h"
#include "Coroutine/LuaCoroutine.h"
#include "GameFramework/Actor.h"
#include "Physics/FCollisionManager.h"
#include "Script/LuaManager.h"
#include "UnrealEd/EditorPlayer.h"
#include "UObject/Casts.h"
#include "Engine/AssetManager.h"
#include "GameFramework/PlayerController.h"
#include "UnrealEd/SkeletalPreviewUI.h"

class ULevel;


FCollisionManager UEditorEngine::CollisionManager;
FCoroutineManager UEditorEngine::CoroutineManager;

void UEditorEngine::Init()
{
    Super::Init();
    LevelEditor = new SLevelEditor();
    UnrealEditor = new UnrealEd();
    ContentsUI = new FContentsUI();
    SkeletalPreviewUI = new FSkeletalPreviewUI();

    UWorld* EditorWorld = CreateWorld(EWorldType::Editor, LEVELTICK_ViewportsOnly);
    EditorWorldContext = CreateNewWorldContext(EditorWorld, EWorldType::Editor, LEVELTICK_ViewportsOnly);
    
    /* must be initialized before window. */
    LevelEditor->Initialize(EditorWorld, GEngineLoop.GetDefaultWindow());

    UnrealEditor->Initialize(LevelEditor, GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenWidth, GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenHeight);
    SkeletalPreviewUI->Initialize(LevelEditor, GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenWidth, GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenHeight);
    ContentsUI->Initialize();
    CollisionManager.Initialize();  
    FLuaManager::Get().Initialize();    

    EditorPlayer = FObjectFactory::ConstructObject<UEditorPlayer>(this);
    EditorPlayer->Initialize();
    
    RegisterWaitHelpers(FLuaManager::Get().GetLuaState());

    if (AssetManager == nullptr)
    {
        AssetManager = FObjectFactory::ConstructObject<UAssetManager>(this);
        assert(AssetManager);
        AssetManager->InitAssetManager();
    }
}

void UEditorEngine::Tick(float DeltaTime)
{
    for (const auto& [key, WorldContext] : WorldContexts)
    {
        WorldContext->GetWorld()->Tick(WorldContext->LevelType, DeltaTime);
    }

    // TODO: 삭제된 오브젝트 참조하는 문제 수정하기
    // CollisionManager.UpdateCollision(DeltaTime);
    
    Input();
    
    LevelEditor->Tick(DeltaTime);

    CoroutineManager.Tick(DeltaTime);
    CoroutineManager.CleanupCoroutines();
}

void UEditorEngine::Release()
{
    LevelEditor->Release();
    
    for (const auto& [key, WorldContext] : WorldContexts)
    {
        WorldContext->GetWorld()->Release();
    }
    WorldContexts.Empty();
    
    
    CollisionManager.Release();

    FLuaManager::Get().Shutdown();
    
    delete LevelEditor;
    delete UnrealEditor;
    delete SkeletalPreviewUI;
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

void UEditorEngine::PreparePIE()
{
    UWorld* PIEWorld = Cast<UWorld>(EditorWorldContext->GetWorld()->Duplicate(this));
    PIEWorld->WorldType = EWorldType::PIE;
    PIEWorld->InitWorld();
    // std::shared_ptr<FWorldContext> PIEWorldContext = CreateNewWorldContext(PIEWorld, EWorldType::PIE, LEVELTICK_All);
    PIEWorldContext = CreateNewWorldContext(PIEWorld, EWorldType::PIE, LEVELTICK_All);
}

void UEditorEngine::StartPIE() const
{
    // 1. BeingPlay() 호출
    PIEWorldContext->GetWorld()->BeginPlay();
    for (const auto& viewportClient: GetLevelEditor()->GetViewportClients(GEngineLoop.GetDefaultWindow()))
    {
        viewportClient->SetWorld(PIEWorldContext->GetWorld());
    }

    // Logo Fade In/Out
    APlayerCameraManager* PlayerCameraManager = PIEWorldContext->GetWorld()->GetPlayerController()->GetPlayerCameraManager();
    UCameraFadeInOut* CameraModifier = FObjectFactory::ConstructObject<UCameraFadeInOut>(PlayerCameraManager);
    CameraModifier->StartFadeIn(0.002f);
    PlayerCameraManager->AddCameraModifier(CameraModifier);

    FGameManager::Get().NPCs.Empty();
    for (int i=0;i < 4;i++)
    {
        ASkeletalMeshActor* NPC = Cast<ASkeletalMeshActor>(PIEWorldContext->GetWorld()->SpawnActor<ASkeletalMeshActor>());
        NPC->SetActorLocation(FVector(0, -80 + 20 * i, 0));
        NPC->SetActorScale(FVector(0.2,0.2,0.2));
        FGameManager::Get().NPCs.Add(NPC);
    }

    FGameManager::Get().StartGame();
    UE_LOG(LogLevel::Display, "Start PIE");
}

void UEditorEngine::PausedPIE() const 
{
    switch (PIEWorldContext->LevelType)
    {
        case LEVELTICK_All:
            PIEWorldContext->LevelType = LEVELTICK_PauseTick;
            break;
        case LEVELTICK_PauseTick:
            PIEWorldContext->LevelType = LEVELTICK_All;
            break;
        default: 
            break;
    }

    UE_LOG(LogLevel::Display, "Pause PIE");
}

void UEditorEngine::ResumingPIE() const
{

}

void UEditorEngine::StopPIE()
{
    if (PIEWorldContext == nullptr)
    {
        MessageBox(nullptr, L"PIE WORLD is not exist", nullptr, MB_OK | MB_ICONERROR);
        return;
    }

    WorldContexts.Remove(PIEWorldContext->GetWorld()->GetUUID());
    for (const auto& ViewportClient: GetLevelEditor()->GetViewportClients(GEngineLoop.GetDefaultWindow()))
    {
        ViewportClient->SetWorld(EditorWorldContext->GetWorld());
    }
    PIEWorldContext->GetWorld()->Release();
    
    
    PIEWorldContext = nullptr;
}

void UEditorEngine::UpdateGizmos(UWorld* World) const
{
    for (UGizmoBaseComponent* GizmoComp : TObjectRange<UGizmoBaseComponent>())
    {
        if (World != GizmoComp->GetWorld())
        {
            continue;
        }
        
        if (!World->GetSelectedActors().IsEmpty())
        {
            const AActor* PickedActor = *World->GetSelectedActors().begin();
            if (PickedActor == nullptr || PickedActor->GetRootComponent() == nullptr)
            {
                break;
            }
            const std::shared_ptr<FEditorViewportClient> ActiveViewport = GetLevelEditor()->GetActiveViewportClient();
            if (ActiveViewport->IsPerspective())
            {
                float ScaleModifier = abs((ActiveViewport->ViewTransformPerspective.GetLocation() - PickedActor->GetRootComponent()->GetRelativeLocation()).Magnitude());
                ScaleModifier *= GizmoComp->GizmoScale;
                GizmoComp->SetRelativeScale(FVector(ScaleModifier));
            }
            else
            {
                const float ScaleModifier = ActiveViewport->OrthoSize * GizmoComp->GizmoScale;
                GizmoComp->SetRelativeScale(FVector(ScaleModifier));
            }
        }
    }
}

UWorld* UEditorEngine::CreateWorld(EWorldType::Type WorldType, ELevelTick LevelTick)
{
    UWorld* World = FObjectFactory::ConstructObject<UWorld>(this);
    World->WorldType = WorldType;
    World->InitWorld();
    return World;
}

void UEditorEngine::RemoveWorld(UWorld* World)
{
    if (World == nullptr)
    {
        return;
    }

    // PreviewWorld인지 검사
    for (const auto& [key, context] : PreviewWorldContexts)
    {
        if (context->GetWorld() == World)
        {
            PreviewWorldContexts.Remove(key);
            break;
        }
    }
    
    World->Release();    
    WorldContexts.Remove(World->GetUUID());
}

UWorld* UEditorEngine::CreatePreviewWindow(const FString& Name)
{
    WCHAR EnginePreviewWindowClass[] = L"PreviewWindowClass";

    // @todo Name 사용하도록 변경
    WCHAR EnginePreviewTitle[256] = L"Preview";

    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(GEngineLoop.GetDefaultWindow(), GWLP_HINSTANCE));
    HWND AppWnd = GEngineLoop.CreateEngineWindow(hInstance, EnginePreviewWindowClass, EnginePreviewTitle);
        
    UWorld* NewPreviewWorld = CreateWorld(EWorldType::EditorPreview, LEVELTICK_All);
    
    // 새 WorldContext 생성
    std::shared_ptr<FWorldContext> PreviewWorldContext = CreateNewWorldContext(NewPreviewWorld, EWorldType::EditorPreview, ELevelTick::LEVELTICK_All);
    
    // PreviewWorldContexts에 추가 (임의의 고유 ID 사용)
    static int PreviewWorldCounter = 0;
    PreviewWorldContexts.Add(PreviewWorldCounter++, PreviewWorldContext);
        
    // 뷰포트 클라이언트 생성 및 설정
    std::shared_ptr<FEditorViewportClient> EditorViewportClient = GetLevelEditor()->AddViewportClient<FEditorViewportClient>(AppWnd, NewPreviewWorld);
    EditorViewportClient->SetViewMode(VMI_Unlit);

    return NewPreviewWorld;
}

std::shared_ptr<FWorldContext> UEditorEngine::CreateNewWorldContext(UWorld* InWorld, EWorldType::Type InWorldType, ELevelTick LevelType)
{
    std::shared_ptr<FWorldContext> NewWorldContext = std::make_shared<FWorldContext>();
    NewWorldContext->WorldType = InWorldType;
    
    WorldContexts.Add(InWorld->GetUUID(), NewWorldContext);
    
    NewWorldContext->LevelType = LevelType;
    NewWorldContext->SetWorld(InWorld);

    return NewWorldContext;
}
