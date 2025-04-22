#include "EditorEngine.h"
#include "ImGuiManager.h"
#include "Engine/World.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/UnrealEd.h"
#include "UnrealClient.h"
#include "Actors/Player.h"
#include "GameFramework/Actor.h"
#include "slate/Widgets/Layout/SSplitter.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/SceneMgr.h"
#include "UObject/UObjectIterator.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "BaseGizmos/TransformGizmo.h"

class ULevel;

FGraphicsDevice UEditorEngine::graphicDevice;
FRenderer UEditorEngine::renderer;
FResourceManager UEditorEngine::resourceMgr;

UEditorEngine::UEditorEngine()
    : hWnd(nullptr)
    , UIMgr(nullptr)
    , GWorld(nullptr)
    , LevelEditor(nullptr)
    , UnrealEditor(nullptr)
{
}


int32 UEditorEngine::Init(HWND hwnd)
{
    /* must be initialized before window. */
    hWnd = hwnd;
    graphicDevice.Initialize(hWnd);
    renderer.Initialize(&graphicDevice);
    UIMgr = new UImGuiManager;
    UIMgr->Initialize(hWnd, graphicDevice.Device, graphicDevice.DeviceContext);
    resourceMgr.Initialize(&renderer, &graphicDevice);

    
    FWorldContext EditorContext;
    EditorContext.WorldType = EWorldType::Editor;
    EditorContext.thisCurrentWorld = FObjectFactory::ConstructObject<UWorld>();
    UWorld* EditWorld  =EditorContext.thisCurrentWorld;
    EditWorld->InitWorld();
    EditWorld->WorldType = EWorldType::Editor;
    GWorld = EditWorld;
    worldContexts.Add(EditorContext);
    
    FWorldContext PIEContext;
    EditorContext.WorldType = EWorldType::PIE;
    worldContexts.Add(PIEContext);
    
    
    LevelEditor = new SLevelEditor();
    LevelEditor->Initialize();
    
    UnrealEditor = new UnrealEd();
    UnrealEditor->Initialize(LevelEditor);
    UnrealEditor->OnResize(hWnd); // 현재 윈도우 사이즈에 대한 재조정
    graphicDevice.OnResize(hWnd);
    
    SceneMgr = new FSceneMgr();

    return 0;
}


void UEditorEngine::Render()
{
    graphicDevice.Prepare();
    if (LevelEditor->IsMultiViewport())
    {
        const std::shared_ptr<FEditorViewportClient> viewportClient = GetLevelEditor()->GetActiveViewportClient();
        for (int i = 0; i < 4; ++i)
        {
            LevelEditor->SetViewportClient(i);
            ResizeGizmo();
            renderer.AddRenderObjectsToRenderPass(GWorld);
            renderer.Render(GWorld, LevelEditor->GetActiveViewportClient());
        }
        GetLevelEditor()->SetViewportClient(viewportClient);
    }
    else
    {
        renderer.AddRenderObjectsToRenderPass(GWorld);
        renderer.Render(GWorld, LevelEditor->GetActiveViewportClient());
    }
    ResizeGizmo();
    renderer.ClearRenderObjects();
}

void UEditorEngine::Tick(float deltaSeconds)
{
    GWorld->Tick(levelType, deltaSeconds);
    Input();
    // GWorld->Tick(LEVELTICK_All, deltaSeconds);
    LevelEditor->Tick(deltaSeconds);
    Render();
    
    UIMgr->BeginFrame();
    UnrealEditor->Render();
    
    Console::GetInstance().Draw();
    
    UIMgr->EndFrame();

    // Pending 처리된 오브젝트 제거
    GUObjectArray.ProcessPendingDestroyObjects();

    graphicDevice.SwapBuffer();
    FVector CurRotation = GetLevelEditor()->GetActiveViewportClient()->ViewTransformPerspective.GetRotation();
   
}

float UEditorEngine::GetAspectRatio(IDXGISwapChain* swapChain) const
{
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain->GetDesc(&desc);
    return static_cast<float>(desc.BufferDesc.Width) / static_cast<float>(desc.BufferDesc.Height);
}

void UEditorEngine::Input()
{
    if (GetAsyncKeyState('M') & 0x8000)
    {
        if (!bTestInput)
        {
            bTestInput = true;
            if (LevelEditor->IsMultiViewport())
            {
                LevelEditor->OffMultiViewport();
            }
            else
                LevelEditor->OnMultiViewport();
        }
    }
    else
    {
        bTestInput = false;
    }
}


void UEditorEngine::PreparePIE()
{
    // 1. World 복제
    worldContexts[1].thisCurrentWorld = Cast<UWorld>(worldContexts[0].thisCurrentWorld->Duplicate());
    GWorld = worldContexts[1].thisCurrentWorld;
    GWorld->WorldType = EWorldType::PIE;
    levelType = LEVELTICK_All;
}

void UEditorEngine::StartPIE()
{
    // 1. BeingPlay() 호출
    GWorld->BeginPlay();
    levelType = LEVELTICK_All;
    UE_LOG(LogLevel::Error, "Start PIE");
}

void UEditorEngine::PausedPIE()
{
    if (levelType == LEVELTICK_All)
        levelType = LEVELTICK_PauseTick;
    else if (levelType == LEVELTICK_PauseTick)
        levelType = LEVELTICK_All;
    UE_LOG(LogLevel::Error, "Pause PIE");
}

void UEditorEngine::ResumingPIE()
{

}

void UEditorEngine::StopPIE()
{
    // 1. World Clear
    GWorld = worldContexts[0].thisCurrentWorld;

    for (auto iter : worldContexts[1].World()->GetActors())
    {
        iter->Destroy();
        GUObjectArray.MarkRemoveObject(iter);
    }
    GUObjectArray.MarkRemoveObject(worldContexts[1].World()->GetLevel());
    worldContexts[1].World()->GetEditorPlayer()->Destroy();
    worldContexts[1].World()->LocalGizmo->Destroy();
    GUObjectArray.MarkRemoveObject( worldContexts[1].World());
    worldContexts[1].thisCurrentWorld = nullptr; 
    
    // GWorld->WorldType = EWorldType::Editor;
    levelType = LEVELTICK_ViewportsOnly;
}

void UEditorEngine::Exit()
{
    LevelEditor->Release();
    GWorld->Release();
    UIMgr->Shutdown();
    delete UIMgr;
    delete SceneMgr;
    resourceMgr.Release(&renderer);
    renderer.Release();
    graphicDevice.Release();
}

void UEditorEngine::ResizeGizmo()
{
    for (auto GizmoComp : TObjectRange<UGizmoBaseComponent>())
    {
        if (!GWorld->GetSelectedActors().IsEmpty())
        {
            AActor* PickedActor = *GWorld->GetSelectedActors().begin();
            if (PickedActor == nullptr)
                break;
            std::shared_ptr<FEditorViewportClient> activeViewport = GetLevelEditor()->GetActiveViewportClient();
            if (activeViewport->IsPerspective())
            {
                float scalar = abs(
                    (activeViewport->ViewTransformPerspective.GetLocation() - PickedActor->GetRootComponent()->GetLocalLocation()).Magnitude()
                );
                scalar *= 0.1f;
                GizmoComp->SetRelativeScale(FVector(scalar, scalar, scalar));
            }
            else
            {
                float scalar = activeViewport->orthoSize * 0.1f;
                GizmoComp->SetRelativeScale(FVector(scalar, scalar, scalar));
            }
        }
    }
}