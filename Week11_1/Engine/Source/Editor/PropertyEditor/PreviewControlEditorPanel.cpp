#include "PreviewControlEditorPanel.h"

#include "Engine/World.h"
#include "Engine/FLoaderOBJ.h"
#include "Engine/StaticMeshActor.h"
#include "LevelEditor/SLevelEditor.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Classes/Actors/DirectionalLightActor.h"
#include "Classes/Actors/PointLightActor.h"
#include "Components/GameFramework/ProjectileMovementComponent.h"
#include "Actors/SpotLightActor.h"
#include <Actors/ExponentialHeightFog.h>
#include <UObject/UObjectIterator.h>

#include "LaunchEngineLoop.h"
#include "ShowFlags.h"
#include "Actors/SkeletalMeshActor.h"
#include "Camera/CameraComponent.h"

#include "Components/PrimitiveComponents/Physics/UBoxShapeComponent.h"
#include "Components/PrimitiveComponents/Physics/USphereShapeComponent.h"
#include "Components/PrimitiveComponents/Physics/UCapsuleShapeComponent.h"
#include "Components/PrimitiveComponents/UParticleSubUVComp.h"
#include "Components/PrimitiveComponents/UTextComponent.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"
#include "Components/USpringArmComponent.h"
#include "Components/Mesh/StaticMesh.h"

#include "Contents/AGPlayer.h"
#include "Font/IconDefs.h"
#include "ImGUI/imgui.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/EditorPlayer.h"
#include "UObject/ObjectTypes.h"


void PreviewControlEditorPanel::Initialize(SLevelEditor* levelEditor, float Width, float Height)
{
    activeLevelEditor = levelEditor;
    this->Width = Width;
    this->Height = Height;
}

void PreviewControlEditorPanel::Render()
{
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();
    ImFont* IconFont = io.Fonts->Fonts[FEATHER_FONT];
    ImVec2 IconSize = ImVec2(32, 32);

    float PanelWidth = (Width) * 0.8f;
    float PanelHeight = 45.0f;

    float PanelPosX = 1.0f;
    float PanelPosY = 1.0f;

    ImVec2 MinSize(300, 50);
    ImVec2 MaxSize(FLT_MAX, 50);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

    /* Render Start */
    ImGui::Begin("Control Panel##", nullptr, PanelFlags);

    CreateMenuButton(IconSize, IconFont);

    ImGui::SameLine();

    CreateFlagButton();

    ImGui::SameLine();

    CreateModifyButton(IconSize, IconFont);

    ImGui::SameLine();

    /* Get Window Content Region */
    float ContentWidth = ImGui::GetWindowContentRegionMax().x;

    /* Move Cursor X Position */
    ImGui::SetCursorPosX(ContentWidth - (IconSize.x * 3.0f + 16.0f));

    ImGui::PushFont(IconFont);
    CreateSRTButton(IconSize);
    ImGui::PopFont();

    ImGui::End();
}

void PreviewControlEditorPanel::CreateMenuButton(ImVec2 ButtonSize, ImFont* IconFont)
{
    ImGui::PushFont(IconFont);
    if (ImGui::Button(ICON_MENU, ButtonSize)) // Menu
    {
        bOpenMenu = !bOpenMenu;
    }
    ImGui::PopFont();

    if (bOpenMenu)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 55), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(135, 170), ImGuiCond_Always);

        ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        if (ImGui::BeginMenu("Import"))
        {
            if (ImGui::MenuItem("Fbx"))
            {
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        // if (ImGui::MenuItem("Quit"))
        // {
        //     // GEngineLoop.DestroyEngineWindow(hWnd, hInstance, ClassName);
        //     // if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        //     // {
        //     //     if (!EditorEngine->GetLevelEditor()->GetViewportClients(hWnd).IsEmpty())
        //     //     {
        //     //         EditorEngine->RemoveWorld(EditorEngine->GetLevelEditor()->GetViewportClients(hWnd)[0]->World);
        //     //     }
        //     //     EditorEngine->GetLevelEditor()->RemoveViewportClients(hWnd);
        //     // }
        //
        //     ImGui::EndMenu();
        // }

        ImGui::End();
    }
}

void PreviewControlEditorPanel::CreateModifyButton(ImVec2 ButtonSize, ImFont* IconFont)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);

    if (EditorEngine == nullptr)
    {
        return;
    }
    
    ImGui::PushFont(IconFont);
    if (ImGui::Button(ICON_SLIDER, ButtonSize)) // Slider
    {
        ImGui::OpenPopup("SliderControl");
    }
    ImGui::PopFont();

    if (ImGui::BeginPopup("SliderControl"))
    {
        ImGui::Text("Grid Scale");
        GridScale = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GridSize;
        ImGui::SetNextItemWidth(120.0f);
        if (ImGui::DragFloat("##Grid Scale", &GridScale, 0.1f, 1.0f, 20.0f, "%.1f"))
        {
            EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GridSize = GridScale;
        }
        ImGui::Separator();

        ImGui::Text("Camera FOV");
        FOV = &EditorEngine->GetLevelEditor()->GetActiveViewportClient()->ViewFOV;
        ImGui::SetNextItemWidth(120.0f);
        if (ImGui::DragFloat("##Fov", FOV, 0.1f, 30.0f, 120.0f, "%.1f"))
        {
            //GEngineLoop.GetWorld()->GetCamera()->SetFOV(FOV);
        }
        ImGui::Spacing();

        ImGui::Text("Camera Speed");
        CameraSpeed = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetCameraSpeedScalar();
        ImGui::SetNextItemWidth(120.0f);
        if (ImGui::DragFloat("##CamSpeed", &CameraSpeed, 0.1f, 0.198f, 192.0f, "%.1f"))
        {
            EditorEngine->GetLevelEditor()->GetActiveViewportClient()->SetCameraSpeed(CameraSpeed);
        }

        ImGui::Text("Blur Strength");
        float BlurStrength = EditorEngine->testBlurStrength;
        ImGui::SetNextItemWidth(120.0f);
        if (ImGui::DragFloat("##Blur Strength", &BlurStrength, 0.02f, 0.0f, 5.0f, "%.1f"))
        {
            EditorEngine->testBlurStrength = BlurStrength;
        }
        ImGui::Separator();

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    ImGui::PushFont(IconFont);
    if (ImGui::Button(ICON_PLUS, ButtonSize))
    {
        ImGui::OpenPopup("ActorControl");
    }
    ImGui::PopFont();

    if (ImGui::BeginPopup("ActorControl"))
    {
        struct Actor {
            const char* category;
            const char* label;
            int obj;
        };

        // 카테고리 순서대로 정렬된 배열
        static const Actor actors[] = {
            { "Defaults", "Actor", OBJ_ACTOR},
            { "Defaults", "GamePlayer", OBJ_GAMEPLAYER},
            // 🔦 라이트
            { "Lights", "Spot Light",      OBJ_SPOTLIGHT },
            { "Lights", "Point Light",     OBJ_POINTLIGHT },
            { "Lights", "Directional Light", OBJ_DIRECTIONALLIGHT },

            // 🔷 셰이프
            { "Shapes", "Cube",            OBJ_CUBE },
            { "Shapes", "Sphere",          OBJ_SPHERE },
            { "Shapes", "Capsule",         OBJ_CAPSULE },
            { "Shapes", "Car (Dodge)",     OBJ_CAR },
            { "Shapes", "SkySphere",       OBJ_SKYSPHERE},
            { "Shapes", "SkeletalMesh",    OBJ_SKELETAL},
            {"Shapes", "Character", OBJ_CHARACTER},

            // ✨ 효과
            { "Effects", "Particle",       OBJ_PARTICLE },
            { "Effects", "Text",           OBJ_TEXT },
            { "Effects", "Fog",            OBJ_FOG },
        };

        const char* currentCategory = nullptr;

        for (const auto& actor : actors)
        {
            // 카테고리 헤더 추가
            if (currentCategory != actor.category)
            {
                if (currentCategory != nullptr)
                {
                    ImGui::Separator(); // 카테고리 구분선
                }
                ImGui::PushFont(IconFont);
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1), ICON_PLAY); // 헤더
                ImGui::PopFont();
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1), "%s", actor.category); // 헤더
                currentCategory = actor.category;
            }

            // 액터 생성 버튼
            if (ImGui::Selectable(actor.label))
            {
                AActor* SpawnedActor = nullptr;

                switch (static_cast<EObjects>(actor.obj))
                {
                case OBJ_ACTOR:
                    SpawnedActor = World->SpawnActor<AActor>();
                    SpawnedActor->SetActorLabel(TEXT("OBJ_ACTOR"));
                    SpawnedActor->AddComponent<USceneComponent>(EComponentOrigin::Editor);
                    break;
                case OBJ_GAMEPLAYER:
                    {
                        SpawnedActor = World->SpawnActor<AGPlayer>();
                        SpawnedActor->SetActorLabel(TEXT("OBJ_GAMEPLAYER"));
                        FManagerOBJ::CreateStaticMesh("Assets/Primitives/Cube.obj");
                        UStaticMeshComponent* MeshComp = SpawnedActor->AddComponent<UStaticMeshComponent>(EComponentOrigin::Editor);
                        MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Cube.obj"));
                        USpringArmComponent* SpringComp = SpawnedActor->AddComponent<USpringArmComponent>(EComponentOrigin::Editor);
                        UCameraComponent* Camera = SpawnedActor->AddComponent<UCameraComponent>(EComponentOrigin::Editor);
                        //SpawnedActor->AddComponent<USphereShapeComponent>(EComponentOrigin::Editor)->SetAttachParent(SpringComp);
                        SpringComp->SetTargetComponent(Camera);

                        SpawnedActor->AddComponent<UBoxShapeComponent>(EComponentOrigin::Editor);
                        break;
                    }
                    //  셰이프
                case OBJ_CUBE:
                    {
                        AStaticMeshActor* TempActor = World->SpawnActor<AStaticMeshActor>();
                        TempActor->SetActorLabel(TEXT("Cube"));
                        UStaticMeshComponent* MeshComp = TempActor->GetStaticMeshComponent();
                        FManagerOBJ::CreateStaticMesh("Assets/Primitives/Cube.obj");
                        MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Cube.obj"));
                        TempActor->AddComponent<UBoxShapeComponent>(EComponentOrigin::Editor);

                        SpawnedActor = TempActor;
                        break;
                    }
                case OBJ_SPHERE:
                    {
                        AStaticMeshActor* TempActor = World->SpawnActor<AStaticMeshActor>();
                        TempActor->SetActorLabel(TEXT("Sphere"));
                        UStaticMeshComponent* MeshComp = TempActor->GetStaticMeshComponent();
                        FManagerOBJ::CreateStaticMesh("Assets/Primitives/Sphere.obj");
                        MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Sphere.obj"));
                        TempActor->AddComponent<USphereShapeComponent>(EComponentOrigin::Editor);

                        SpawnedActor = TempActor;
                        break;
                    }
                case OBJ_CAPSULE:
                    {
                        AStaticMeshActor* TempActor = World->SpawnActor<AStaticMeshActor>();
                        TempActor->SetActorLabel(TEXT("Capsule"));
                        UStaticMeshComponent* MeshComp = TempActor->GetStaticMeshComponent();
                        FManagerOBJ::CreateStaticMesh("Assets/Primitives/Capsule.obj");
                        MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Capsule.obj"));
                        TempActor->AddComponent<UCapsuleShapeComponent>(EComponentOrigin::Editor);

                        SpawnedActor = TempActor;
                        break;
                    }
                case OBJ_SKYSPHERE:
                    {
                        AStaticMeshActor* TempActor = World->SpawnActor<AStaticMeshActor>();
                        TempActor->SetActorLabel(TEXT("OBJ_SKYSPHERE"));
                        UStaticMeshComponent* MeshComp = TempActor->GetStaticMeshComponent();
                        FManagerOBJ::CreateStaticMesh("Assets/SkySphere.obj");
                        MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"SkySphere.obj"));
                        MeshComp->GetStaticMesh()->GetMaterials()[0]->Material->SetDiffuse(FVector::OneVector);
                        MeshComp->GetStaticMesh()->GetMaterials()[0]->Material->SetEmissive(FVector::OneVector);
                        TempActor->SetActorRotation(FRotator(0.0f, 0.0f, 90.0f));
                        TempActor->SetActorScale(FVector(1.0f, 1.0f, 1.0f));
                        SpawnedActor = TempActor;

                        // AGBullet* TempActor = World->SpawnActor<AGBullet>();
                        // TempActor->SetActorLabel(TEXT("Bullet"));
                        //
                        // SpawnedActor = TempActor;

                        break; // 누락된 break 추가
                    }
                case OBJ_SKELETAL:
                    {
                        ASkeletalMeshActor* skeletalMeshActor = World->SpawnActor<ASkeletalMeshActor>();
                        skeletalMeshActor->SetActorLabel("SkeletalMesh");
                        SpawnedActor = skeletalMeshActor;
                        break;
                    }
                case OBJ_POINTLIGHT:
                    {
                        SpawnedActor = World->SpawnActor<APointLightActor>();
                        SpawnedActor->SetActorLabel(TEXT("OBJ_POINTLIGHT"));
                        break;
                    }
                case OBJ_SPOTLIGHT:
                    {
                        SpawnedActor = World->SpawnActor<ASpotLightActor>();
                        SpawnedActor->SetActorLabel(TEXT("OBJ_SpotLight"));
                        break;
                    }
                case OBJ_DIRECTIONALLIGHT:
                    {
                        SpawnedActor = World->SpawnActor<ADirectionalLightActor>();
                        SpawnedActor->SetActorLabel(TEXT("OBJ_DIRECTIONALLIGHT"));
                        break;
                    }

                    // ✨ 효과
                case OBJ_PARTICLE:
                    {
                        SpawnedActor = World->SpawnActor<AActor>();
                        SpawnedActor->SetActorLabel(TEXT("OBJ_PARTICLE"));
                        UParticleSubUVComp* Particle = SpawnedActor->AddComponent<UParticleSubUVComp>(EComponentOrigin::Editor);
                        Particle->SetTexture(L"Assets/Texture/T_Explosion_SubUV.png");
                        Particle->SetRowColumnCount(6, 6);
                        Particle->SetRelativeScale(FVector(10.0f, 10.0f, 10.0f));
                        Particle->Activate();
                        break;
                    }
                case OBJ_TEXT:
                    {
                        SpawnedActor = World->SpawnActor<AActor>();
                        SpawnedActor->SetActorLabel(TEXT("OBJ_TEXT"));
                        UTextComponent* Text = SpawnedActor->AddComponent<UTextComponent>(EComponentOrigin::Editor);
                        Text->SetTexture(L"Assets/Texture/font.png");
                        Text->SetRowColumnCount(106, 106);
                        Text->SetText(L"안녕하세요 Jungle 1");
                        break;
                    }
                case OBJ_FOG:
                    {
                        for (const auto& Actor : TObjectRange<AExponentialHeightFogActor>())
                        {
                            if (Actor)
                            {
                                if (Actor->GetWorld() != World)
                                {
                                    continue;
                                }
                                Actor->Destroy();
                                TSet<AActor*> Actors = World->GetSelectedActors();
                                if(Actors.Contains(Actor))
                                {
                                    World->ClearSelectedActors();
                                }
                            }
                        }
                        SpawnedActor = World->SpawnActor<AExponentialHeightFogActor>();
                        SpawnedActor->SetActorLabel(TEXT("OBJ_FOG"));
                        break;
                    }
                case OBJ_CAR:
                    {
                        AStaticMeshActor* TempActor = World->SpawnActor<AStaticMeshActor>();
                        TempActor->SetActorLabel(TEXT("OBJ_DODGE"));
                        UStaticMeshComponent* MeshComp = TempActor->GetStaticMeshComponent();
                        FManagerOBJ::CreateStaticMesh("Assets/Dodge/Dodge.obj");
                        MeshComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Dodge.obj"));
                        SpawnedActor = TempActor;
                        break;
                    }
                }

                if (SpawnedActor)
                {
                    World->SetSelectedActor(SpawnedActor);
                }
            }
        }
        ImGui::EndPopup();
    }
}

void PreviewControlEditorPanel::CreateFlagButton() const
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);

    if (EditorEngine == nullptr)
    {
        return;
    }
    auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    const char* ViewTypeNames[] = { "Perspective", "Top", "Bottom", "Left", "Right", "Front", "Back" };
    ELevelViewportType ActiveViewType = ActiveViewport->GetViewportType();
    FString TextViewType = ViewTypeNames[ActiveViewType];

    if (ImGui::Button(GetData(TextViewType), ImVec2(120, 32)))
    {
        // toggleViewState = !toggleViewState;
        ImGui::OpenPopup("ViewControl");
    }

    if (ImGui::BeginPopup("ViewControl"))
    {
        for (int i = 0; i < IM_ARRAYSIZE(ViewTypeNames); i++)
        {
            bool bIsSelected = ((int)ActiveViewport->GetViewportType() == i);
            if (ImGui::Selectable(ViewTypeNames[i], bIsSelected))
            {
                ActiveViewport->SetViewportType((ELevelViewportType)i);
            }

            if (bIsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine();

    const char* ViewModeNames[] = { "Goroud_Lit", "Lambert_Lit", "Phong_Lit", "Unlit", "Wireframe", "Depth", "Normal"};
    FString SelectLightControl = ViewModeNames[static_cast<uint32>(ActiveViewport->GetViewMode())];
    ImVec2 LightTextSize = ImGui::CalcTextSize(GetData(SelectLightControl));

    if (ImGui::Button(GetData(SelectLightControl), ImVec2(30 + LightTextSize.x, 32)))
    {
        ImGui::OpenPopup("LightControl");
    }

    if (ImGui::BeginPopup("LightControl"))
    {
        for (int i = 0; i < IM_ARRAYSIZE(ViewModeNames); i++)
        {
            const bool bIsSelected = (static_cast<uint32>(ActiveViewport->GetViewMode()) == i);
            if (ImGui::Selectable(ViewModeNames[i], bIsSelected))
            {
                ActiveViewport->SetViewMode(static_cast<EViewModeIndex>(i));
                //UEditorEngine::renderer.SetViewMode(ActiveViewport->GetViewMode());
            }

            if (bIsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Show", ImVec2(60, 32)))
    {
        ImGui::OpenPopup("ShowControl");
    }

    const char* items[] = { "AABB", "Primitive", "BillBoard", "UUID", "Fog", "SkeletalMesh" };
    uint64 ActiveViewportFlags = ActiveViewport->GetShowFlag();

    if (ImGui::BeginPopup("ShowControl"))
    {
        bool selected[IM_ARRAYSIZE(items)] =
        {
            (ActiveViewportFlags & static_cast<uint64>(EEngineShowFlags::SF_AABB)) != 0,
            (ActiveViewportFlags & static_cast<uint64>(EEngineShowFlags::SF_Primitives)) != 0,
            (ActiveViewportFlags & static_cast<uint64>(EEngineShowFlags::SF_BillboardText)) != 0,
            (ActiveViewportFlags & static_cast<uint64>(EEngineShowFlags::SF_UUIDText)) != 0,
            (ActiveViewportFlags & static_cast<uint64>(EEngineShowFlags::SF_Fog)) != 0,
            (ActiveViewportFlags & static_cast<uint64>(EEngineShowFlags::SF_SkeletalMesh)) != 0,
        };  // 각 항목의 체크 상태 저장

        for (int i = 0; i < IM_ARRAYSIZE(items); i++)
        {
            ImGui::Checkbox(items[i], &selected[i]);
        }
        ActiveViewport->SetShowFlag(ConvertSelectionToFlags(selected));
        ImGui::EndPopup();
    }
}

void PreviewControlEditorPanel::CreateShaderHotReloadButton(const ImVec2 ButtonSize) const
{
    ID3D11ShaderResourceView* IconTextureSRV = GEngineLoop.ResourceManager.GetTexture(L"Assets/Texture/HotReload.png")->TextureSRV;
    const ImTextureID textureID = reinterpret_cast<ImTextureID>(IconTextureSRV); // 실제 사용되는 텍스처 SRV
    if (ImGui::ImageButton("btn1", textureID, ButtonSize))
    {
        GEngineLoop.Renderer.GetResourceManager()->HotReloadShaders();
    }
}

void PreviewControlEditorPanel::CreatePIEButton(ImVec2 ButtonSize) const
{
    float TotalWidth = ButtonSize.x * 3.0f + 16.0f;
    float ContentWidth = ImGui::GetWindowContentRegionMax().x;

    // 중앙 정렬을 위한 커서 위치 설정
    float CursorPosX = (ContentWidth - TotalWidth) * 0.5f;
    ImGui::SetCursorPosX(CursorPosX);

    if (activeLevelEditor->GetEditorStateManager().GetEditorState() == EEditorState::Editing)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        if (ImGui::Button(ICON_PLAY, ButtonSize)) // Play
        {
            activeLevelEditor->GetEditorStateManager().SetState(EEditorState::PreparingPlay);
        }
        ImGui::PopStyleColor();
    }
    else if (activeLevelEditor->GetEditorStateManager().GetEditorState() == EEditorState::Paused)
    {
        if (ImGui::Button(ICON_PLAY, ButtonSize)) // Play
        {
            activeLevelEditor->GetEditorStateManager().SetState(EEditorState::Playing);
        }
    }
    else
    {
        if (ImGui::Button(ICON_PAUSE, ButtonSize)) // Pause
        {
            // TODO: PIE 일시정지
            activeLevelEditor->GetEditorStateManager().SetState(EEditorState::Paused);
        }
    }
    ImGui::SameLine();

    if (activeLevelEditor->GetEditorStateManager().GetEditorState() == EEditorState::Editing)
    {
        if (ImGui::Button(ICON_STOP, ButtonSize)) // Stop
        {
            activeLevelEditor->GetEditorStateManager().SetState(EEditorState::Stopped);
        }
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button(ICON_STOP, ButtonSize)) // Stop
        {
            activeLevelEditor->GetEditorStateManager().SetState(EEditorState::Stopped);
        }
        ImGui::PopStyleColor();
    }
}

// code is so dirty / Please refactor
void PreviewControlEditorPanel::CreateSRTButton(ImVec2 ButtonSize) const
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);

    if (EditorEngine == nullptr)
    {
        return;
    }
    
    SLevelEditor* LevelEditor = EditorEngine->GetLevelEditor();

    ImVec4 ActiveColor = ImVec4(0.00f, 0.00f, 0.85f, 1.0f);

    EControlMode ControlMode = LevelEditor->GetActiveViewportClientData().GetControlMode();

    if (ControlMode == CM_TRANSLATION)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ActiveColor);
    }
    if (ImGui::Button(ICON_MOVE, ButtonSize)) // Move
    {
        LevelEditor->GetActiveViewportClientData().SetMode(CM_TRANSLATION);
    }
    if (ControlMode == CM_TRANSLATION)
    {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();

    if (ControlMode == CM_ROTATION)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ActiveColor);
    }
    if (ImGui::Button(ICON_ROTATE, ButtonSize)) // Rotate
    {
        LevelEditor->GetActiveViewportClientData().SetMode(CM_ROTATION);
    }
    if (ControlMode == CM_ROTATION)
    {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();

    if (ControlMode == CM_SCALE)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ActiveColor);
    }
    if (ImGui::Button(ICON_SCALE, ButtonSize)) // Scale
    {
        LevelEditor->GetActiveViewportClientData().SetMode(CM_SCALE);
    }
    if (ControlMode == CM_SCALE)
    {
        ImGui::PopStyleColor();
    }
}

uint64 PreviewControlEditorPanel::ConvertSelectionToFlags(const bool selected[]) const
{
    uint64 flags = static_cast<uint64>(EEngineShowFlags::None);

    if (selected[0])
        flags |= static_cast<uint64>(EEngineShowFlags::SF_AABB);
    if (selected[1])
        flags |= static_cast<uint64>(EEngineShowFlags::SF_Primitives);
    if (selected[2])
        flags |= static_cast<uint64>(EEngineShowFlags::SF_BillboardText);
    if (selected[3])
        flags |= static_cast<uint64>(EEngineShowFlags::SF_UUIDText);
    if (selected[4])
        flags |= static_cast<uint64>(EEngineShowFlags::SF_Fog);
    if (selected[5])
        flags |= static_cast<uint64>(EEngineShowFlags::SF_SkeletalMesh);
    return flags;
}


void PreviewControlEditorPanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    Width = clientRect.right - clientRect.left;
    Height = clientRect.bottom - clientRect.top;
}
