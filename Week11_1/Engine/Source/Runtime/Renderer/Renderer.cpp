#include "Renderer.h"
#include <d3dcompiler.h>

#include "ShowFlags.h"
#include "VBIBTopologyMapping.h"
#include "ComputeShader/ComputeTileLightCulling.h"
#include "Engine/World.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealEd/EditorViewportClient.h"
#include "D3D11RHI/FShaderProgram.h"
#include "Engine/Engine.h"
#include "Engine/FEditorStateManager.h"
#include "RenderPass/BlurRenderPass.h"
#include "RenderPass/EditorIconRenderPass.h"
#include "RenderPass/FadeRenderPass.h"
#include "RenderPass/FinalRenderPass.h"
#include "RenderPass/GizmoRenderPass.h"
#include "RenderPass/LetterBoxRenderPass.h"
#include "RenderPass/LineBatchRenderPass.h"
#include "RenderPass/ParticleRenderPass.h"
#include "RenderPass/SkeletalMeshRenderPass.h"
#include "RenderPass/StaticMeshRenderPass.h"

D3D_SHADER_MACRO FRenderer::GouradDefines[] =
{
    {"LIGHTING_MODEL_GOURAUD", "1"},
    {nullptr, nullptr}
};

D3D_SHADER_MACRO FRenderer::LambertDefines[] = 
{
    {"LIGHTING_MODEL_LAMBERT", "1"},
    {nullptr, nullptr}
};

D3D_SHADER_MACRO FRenderer::EditorGizmoDefines[] = 
{
    {"RENDER_GIZMO", "1"},
    {nullptr, nullptr}
};

D3D_SHADER_MACRO FRenderer::EditorIconDefines[] = 
{
    {"RENDER_ICON", "1"},
    {nullptr, nullptr}
};

D3D_SHADER_MACRO FRenderer::SpriteParticleDefines[] =
{
    {"SPRITE_PARTICLE", "1"},
    {nullptr, nullptr}
};

D3D_SHADER_MACRO FRenderer::MeshParticleDefines[] =
{
    {"MESH_PARTICLE", "1"},
    {nullptr, nullptr}
};

void FRenderer::Initialize(FGraphicsDevice* graphics)
{
    Graphics = graphics;
    RenderResourceManager = new FRenderResourceManager(Graphics);
    RenderResourceManager->Initialize();

    //CreateComputeShader();
    
    D3D_SHADER_MACRO defines[] = 
    {
        {"LIGHTING_MODEL_GOURAUD", "1"},
        {nullptr, nullptr}
    };
    //SetViewMode(VMI_Lit_Phong);
    
    CreateVertexPixelShader(TEXT("UberLit"), GouradDefines);
    FString GouradShaderName = TEXT("UberLit");
    GouradShaderName += GouradDefines->Name;
    GoroudRenderPass = std::make_shared<FStaticMeshRenderPass>(GouradShaderName);

    CreateVertexPixelShader(TEXT("UberLit"), LambertDefines);
    FString LamberShaderName = TEXT("UberLit");
    LamberShaderName += LambertDefines->Name;
    LambertRenderPass = std::make_shared<FStaticMeshRenderPass>(LamberShaderName);
    
    CreateVertexPixelShader(TEXT("UberLit"), nullptr);
    FString PhongShaderName = TEXT("UberLit");
    PhongRenderPass = std::make_shared<FStaticMeshRenderPass>(PhongShaderName);

    CreateVertexPixelShader(TEXT("SkeletalUberLit"), nullptr);
    FString SkeletalShaderName = TEXT("SkeletalUberLit");
    SkeletalRenderPass = std::make_shared<FSkeletalMeshRenderPass>(SkeletalShaderName);
    
    CreateVertexPixelShader(TEXT("Line"), nullptr);
    LineBatchRenderPass = std::make_shared<FLineBatchRenderPass>(TEXT("Line"));

    CreateVertexPixelShader(TEXT("DebugDepth"), nullptr);
    DebugDepthRenderPass = std::make_shared<FDebugDepthRenderPass>(TEXT("DebugDepth"));
    
    FString GizmoShaderName = TEXT("Editor");
    GizmoShaderName += EditorGizmoDefines->Name;
    CreateVertexPixelShader(TEXT("Editor"), EditorGizmoDefines);
    GizmoRenderPass = std::make_shared<FGizmoRenderPass>(GizmoShaderName);
    
    FString IconShaderName = TEXT("Editor");
    IconShaderName += EditorIconDefines->Name;
    CreateVertexPixelShader(TEXT("Editor"), EditorIconDefines);
    EditorIconRenderPass = std::make_shared<FEditorIconRenderPass>(IconShaderName);

    CreateVertexPixelShader(TEXT("HeightFog"), nullptr);
    FogRenderPass = std::make_shared<FFogRenderPass>(TEXT("HeightFog"));

    CreateVertexPixelShader(TEXT("Shadow"), nullptr);
    ShadowRenderPass = std::make_shared<FShadowRenderPass>(TEXT("Shadow"));

    CreateVertexPixelShader(TEXT("LightDepth"), nullptr);

    CreateVertexPixelShader(TEXT("Fade"), nullptr);
    FadeRenderPass = std::make_shared<FFadeRenderPass>(TEXT("Fade"));

    CreateVertexPixelShader(TEXT("LetterBox"), nullptr);
    LetterBoxRenderPass = std::make_shared<FLetterBoxRenderPass>(TEXT("LetterBox"));

    CreateVertexPixelShader(TEXT("Blur"), nullptr);
    BlurRenderPass = std::make_shared<FBlurRenderPass>(TEXT("Blur"));
    
    CreateVertexPixelShader(TEXT("Final"), nullptr);
    FinalRenderPass = std::make_shared<FFinalRenderPass>(TEXT("Final"));

    CreateVertexPixelShader(TEXT("ParticleSystem"), nullptr);
    ParticleRenderPass = std::make_shared<FParticleRenderPass>(TEXT("ParticleSystem"));
    ParticleRenderPass->RenderPassEmitterType = DET_Sprite;
    
    FString MeshParticleName = TEXT("ParticleSystem");
    MeshParticleName += MeshParticleDefines->Name;
    CreateVertexPixelShader(TEXT("ParticleSystem"), MeshParticleDefines);
    MeshParticleRenderPass = std::make_shared<FParticleRenderPass>(MeshParticleName);
    MeshParticleRenderPass->RenderPassEmitterType = DET_Mesh;

    //Particle은 Instancing해야해서 InputLayout 따로 만들어야함
    ID3D11InputLayout* ParticleInputLayout = nullptr;
    ID3DBlob* ParticleVertexBlob = RenderResourceManager->GetVertexShaderBlob(GetVSName(TEXT("ParticleSystem")));
    D3D11_INPUT_ELEMENT_DESC ParticleInputLayoutDesc[] =
    {
        // 기본 쿼드 정점
        {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    
        // 인스턴스 데이터
        {"INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCESIZE", 0, DXGI_FORMAT_R32G32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCEROTATION", 0, DXGI_FORMAT_R32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCESUBINDEX", 0, DXGI_FORMAT_R32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    };
    Graphics->Device->CreateInputLayout(ParticleInputLayoutDesc, ARRAYSIZE(ParticleInputLayoutDesc),
        ParticleVertexBlob->GetBufferPointer(), ParticleVertexBlob->GetBufferSize(), &ParticleInputLayout);
    RenderResourceManager->AddOrSetInputLayout(TEXT("SpriteParticle"), ParticleInputLayout);

    ID3D11InputLayout* MeshParticleInputLayout = nullptr;
    ID3DBlob* MeshParticleVertexBlob = RenderResourceManager->GetVertexShaderBlob(GetVSName(MeshParticleName));
    D3D11_INPUT_ELEMENT_DESC MeshParticleInputLayoutDesc[] =
    {
        // 기본 쿼드 정점
        {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    
        // 인스턴스 데이터
        {"INSTANCEMODELMAT1", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCEMODELMAT2", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCEMODELMAT3", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCEMODELMAT4", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    };
    Graphics->Device->CreateInputLayout(MeshParticleInputLayoutDesc, ARRAYSIZE(MeshParticleInputLayoutDesc),
        MeshParticleVertexBlob->GetBufferPointer(), MeshParticleVertexBlob->GetBufferSize(), &MeshParticleInputLayout);

    RenderResourceManager->AddOrSetInputLayout(TEXT("MeshParticle"), MeshParticleInputLayout);
}

void FRenderer::PrepareShader(const FName InShaderName)
{
    ShaderPrograms[InShaderName]->Bind();

    BindConstantBuffers(InShaderName);
}

void FRenderer::BindConstantBuffers(const FName InShaderName)
{
    TMap<FShaderConstantKey, uint32> curShaderBindedConstant = ShaderConstantNameAndSlots[InShaderName];
    for (const auto item : curShaderBindedConstant)
    {
        auto curConstantBuffer = RenderResourceManager->GetConstantBuffer(item.Key.ConstantName);
        if (item.Key.ShaderType == EShaderStage::VS)
        {
            if (curConstantBuffer)
                Graphics->DeviceContext->VSSetConstantBuffers(item.Value, 1, &curConstantBuffer);
        }
        else if (item.Key.ShaderType == EShaderStage::PS)
        {
            if (curConstantBuffer)
                Graphics->DeviceContext->PSSetConstantBuffers(item.Value, 1, &curConstantBuffer);
        }
    }
}

void FRenderer::CreateMappedCB(TMap<FShaderConstantKey, uint32>& ShaderStageToCB, const TArray<FConstantBufferInfo>& CBArray, const EShaderStage Stage) const
{
    for (const FConstantBufferInfo& item : CBArray)
    {
        ShaderStageToCB[{Stage, item.Name}] = item.BindSlot;
        if (RenderResourceManager->GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager->CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager->AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }
}

void FRenderer::Release()
{
    RenderResourceManager->ReleaseResources();
    
    delete RenderResourceManager;
    RenderResourceManager = nullptr;
    
    for (const auto item : ShaderPrograms)
    {
        item.Value->Release();
    }
}

void FRenderer::CreateVertexPixelShader(const FString& InPrefix, D3D_SHADER_MACRO* pDefines)
{
    FString Prefix = InPrefix;
    if (pDefines != nullptr)
    {
#if USE_WIDECHAR
        Prefix += ConvertAnsiToWchar(pDefines->Name);
#else
        Prefix += pDefines->Name;
#endif
    }
    // 접미사를 각각 붙여서 전체 파일명 생성
    const FString VertexShaderFile = InPrefix + TEXT("VertexShader.hlsl");
    const FString PixelShaderFile  = InPrefix + TEXT("PixelShader.hlsl");

    const FString VertexShaderName = Prefix+ TEXT("VertexShader.hlsl");
    const FString PixelShaderName = Prefix+ TEXT("PixelShader.hlsl");
    
    RenderResourceManager->CreateVertexShader(VertexShaderName, VertexShaderFile, pDefines);
    RenderResourceManager->CreatePixelShader(PixelShaderName, PixelShaderFile, pDefines);

    ID3DBlob* VertexShaderBlob = RenderResourceManager->GetVertexShaderBlob(VertexShaderName);
    
    TArray<FConstantBufferInfo> VertexStaticMeshConstant;
    ID3D11InputLayout* InputLayout = nullptr;
    Graphics->ExtractVertexShaderInfo(VertexShaderBlob, VertexStaticMeshConstant, InputLayout);
    RenderResourceManager->AddOrSetInputLayout(VertexShaderName, InputLayout);

    ID3DBlob* PixelShaderBlob = RenderResourceManager->GetPixelShaderBlob(PixelShaderName);
    TArray<FConstantBufferInfo> PixelStaticMeshConstant;
    Graphics->ExtractPixelShaderInfo(PixelShaderBlob, PixelStaticMeshConstant);
    
    TMap<FShaderConstantKey, uint32> ShaderStageToCB;

    CreateMappedCB(ShaderStageToCB, VertexStaticMeshConstant, EShaderStage::VS);  
    CreateMappedCB(ShaderStageToCB, PixelStaticMeshConstant, EShaderStage::PS);
    
    MappingVSPSInputLayout(Prefix, VertexShaderName, PixelShaderName, VertexShaderName);
    MappingVSPSCBSlot(Prefix, ShaderStageToCB);
}

#pragma region Shader

//void FRenderer::CreateComputeShader()
//{
//    ID3DBlob* CSBlob_LightCulling = nullptr;
//    
//    ID3D11ComputeShader* ComputeShader = RenderResourceManager->GetComputeShader(TEXT("TileLightCulling"));
//    
//    if (ComputeShader == nullptr)
//    {
//        Graphics->CreateComputeShader(TEXT("TileLightCulling.compute"), nullptr, &CSBlob_LightCulling, &ComputeShader);
//    }
//    else
//    {
//        FGraphicsDevice::CompileComputeShader(TEXT("TileLightCulling.compute"), nullptr,  &CSBlob_LightCulling);
//    }
//    RenderResourceManager->AddOrSetComputeShader(TEXT("TileLightCulling"), ComputeShader);
//    
//    TArray<FConstantBufferInfo> LightCullingComputeConstant;
//    Graphics->ExtractPixelShaderInfo(CSBlob_LightCulling, LightCullingComputeConstant);
//    
//    TMap<FShaderConstantKey, uint32> ShaderStageToCB;
//
//    for (const FConstantBufferInfo item : LightCullingComputeConstant)
//    {
//        ShaderStageToCB[{EShaderStage::CS, item.Name}] = item.BindSlot;
//        if (RenderResourceManager->GetConstantBuffer(item.Name) == nullptr)
//        {
//            ID3D11Buffer* ConstantBuffer = RenderResourceManager->CreateConstantBuffer(item.ByteWidth);
//            RenderResourceManager->AddOrSetConstantBuffer(item.Name, ConstantBuffer);
//        }
//    }
//
//    MappingVSPSCBSlot(TEXT("TileLightCulling"), ShaderStageToCB);
//    
//    ComputeTileLightCulling = std::make_shared<FComputeTileLightCulling>(TEXT("TileLightCulling"));
//
//    SAFE_RELEASE(CSBlob_LightCulling)
//}

void FRenderer::Render(const std::shared_ptr<FEditorViewportClient>& ActiveViewportClient)
{
    AddRenderObjectsToRenderPass(ActiveViewportClient->GetWorld());
    SetViewMode(ActiveViewportClient->GetViewMode());
    Graphics->DeviceContext->RSSetViewports(1, &ActiveViewportClient->GetD3DViewport());
    
    LineBatchRenderPass->Prepare(ActiveViewportClient);
    LineBatchRenderPass->Execute(ActiveViewportClient);
    
    if (ActiveViewportClient->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))
    {
        ShadowRenderPass->Prepare(ActiveViewportClient);
        ShadowRenderPass->Execute(ActiveViewportClient);

        LineBatchRenderPass->Prepare(ActiveViewportClient);
        LineBatchRenderPass->Execute(ActiveViewportClient);

        //TODO : FLAG로 나누기
        if (CurrentViewMode  == EViewModeIndex::VMI_Lit_Goroud)
        {
            GoroudRenderPass->Prepare(ActiveViewportClient);
            GoroudRenderPass->Execute(ActiveViewportClient);
        }
        else if (CurrentViewMode  == EViewModeIndex::VMI_Lit_Lambert)
        {
            LambertRenderPass->Prepare(ActiveViewportClient);
            LambertRenderPass->Execute(ActiveViewportClient);
        }
        else
        {
            PhongRenderPass->Prepare(ActiveViewportClient);
            PhongRenderPass->Execute(ActiveViewportClient);
        }

        if (ActiveViewportClient->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_SkeletalMesh))
        {
            SkeletalRenderPass->Prepare(ActiveViewportClient);
            SkeletalRenderPass->Execute(ActiveViewportClient);
        }
        
        // if (ActiveViewportClient->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Particle))
        {
            ParticleRenderPass->Prepare(ActiveViewportClient);
            ParticleRenderPass->Execute(ActiveViewportClient);
        }

        {
            MeshParticleRenderPass->Prepare(ActiveViewportClient);
            MeshParticleRenderPass->Execute(ActiveViewportClient);
        }
    }

    if (FogRenderPass->ShouldRender())
    {
        FogRenderPass->Prepare(ActiveViewportClient);
        FogRenderPass->Execute(ActiveViewportClient);
    }

    BlurRenderPass->Prepare(ActiveViewportClient);
    BlurRenderPass->Execute(ActiveViewportClient);

    if (ActiveViewportClient->GetViewMode() == EViewModeIndex::VMI_Depth)
    {
        DebugDepthRenderPass->Prepare(ActiveViewportClient);
        DebugDepthRenderPass->Execute(ActiveViewportClient);
    }
    
    EditorIconRenderPass->Prepare(ActiveViewportClient);
    EditorIconRenderPass->Execute(ActiveViewportClient);

    if (!ActiveViewportClient->GetWorld()->GetSelectedActors().IsEmpty())
    {
        GizmoRenderPass->Prepare(ActiveViewportClient);
        GizmoRenderPass->Execute(ActiveViewportClient);
    }
    
    LetterBoxRenderPass->Prepare(ActiveViewportClient);
    LetterBoxRenderPass->Execute(ActiveViewportClient);
    
    FadeRenderPass->Prepare(ActiveViewportClient);
    FadeRenderPass->Execute(ActiveViewportClient);

    FinalRenderPass->Prepare(ActiveViewportClient);
    FinalRenderPass->Execute(ActiveViewportClient);

    EndRender();
}

void FRenderer::EndRender() const
{
    ClearRenderObjects();
}

void FRenderer::ClearRenderObjects() const
{
    GoroudRenderPass->ClearRenderObjects();
    LambertRenderPass->ClearRenderObjects();
    PhongRenderPass->ClearRenderObjects();
    SkeletalRenderPass->ClearRenderObjects();
    LineBatchRenderPass->ClearRenderObjects();
    GizmoRenderPass->ClearRenderObjects();
    DebugDepthRenderPass->ClearRenderObjects();
    EditorIconRenderPass->ClearRenderObjects();
    ShadowRenderPass->ClearRenderObjects();
    FadeRenderPass->ClearRenderObjects();
    LetterBoxRenderPass->ClearRenderObjects();
    FogRenderPass->ClearRenderObjects();
    BlurRenderPass->ClearRenderObjects();
    FinalRenderPass->ClearRenderObjects();
    ParticleRenderPass->ClearRenderObjects();
}

void FRenderer::SetViewMode(const EViewModeIndex evi)
{
    switch (evi)
    {
    case EViewModeIndex::VMI_Lit_Goroud:
        CurrentRasterizerState = ERasterizerState::SolidBack;
        CurrentViewMode = VMI_Lit_Goroud;
        //TODO : Light 받는 거
        bIsLit = true;
        bIsNormal = false;
        break;
    case EViewModeIndex::VMI_Lit_Lambert:
        CurrentRasterizerState = ERasterizerState::SolidBack;
        CurrentViewMode = VMI_Lit_Lambert;
        bIsLit = true;
        bIsNormal = false;
        break;
    case EViewModeIndex::VMI_Lit_Phong:
        CurrentRasterizerState = ERasterizerState::SolidBack;
        CurrentViewMode = VMI_Lit_Phong;
        bIsLit = true;
        bIsNormal = false;
        break;
    case EViewModeIndex::VMI_Wireframe:
        CurrentRasterizerState = ERasterizerState::WireFrame;
        CurrentViewMode = VMI_Wireframe;
        bIsLit = false;
        bIsNormal = false;
        break;
    case EViewModeIndex::VMI_Unlit:
        CurrentRasterizerState = ERasterizerState::SolidBack;
        CurrentViewMode = VMI_Unlit;
        //TODO : Light 안 받는 거
        bIsLit = false;
        bIsNormal = false;
        break;
    case EViewModeIndex::VMI_Depth:
        CurrentRasterizerState = ERasterizerState::SolidBack;
        CurrentViewMode = VMI_Depth;
        break;
    case EViewModeIndex::VMI_Normal:
        CurrentRasterizerState = ERasterizerState::SolidBack;
        CurrentViewMode = VMI_Normal;
        bIsLit = false;
        bIsNormal = true;
        break;
    default:
        CurrentRasterizerState = ERasterizerState::SolidBack;
        CurrentViewMode = VMI_Lit_Phong;
        bIsLit = true;
        bIsNormal = false;
        break;
    }
}

void FRenderer::AddRenderObjectsToRenderPass(UWorld* World) const
{
    //ComputeTileLightCulling->AddRenderObjectsToRenderPass(InWorld);

    GoroudRenderPass->AddRenderObjectsToRenderPass(World);
    LambertRenderPass->AddRenderObjectsToRenderPass(World);
    PhongRenderPass->AddRenderObjectsToRenderPass(World);
    SkeletalRenderPass->AddRenderObjectsToRenderPass(World);
    
    GizmoRenderPass->AddRenderObjectsToRenderPass(World);
    EditorIconRenderPass->AddRenderObjectsToRenderPass(World);
    ShadowRenderPass->AddRenderObjectsToRenderPass(World);

    LineBatchRenderPass->AddRenderObjectsToRenderPass(World);
    FadeRenderPass->AddRenderObjectsToRenderPass(World);
    LetterBoxRenderPass->AddRenderObjectsToRenderPass(World);
    FogRenderPass->AddRenderObjectsToRenderPass(World);
    BlurRenderPass->AddRenderObjectsToRenderPass(World);
    FinalRenderPass->AddRenderObjectsToRenderPass(World);

    ParticleRenderPass->AddRenderObjectsToRenderPass(World);
}

FName FRenderer::GetVSName(const FName InShaderProgramName)
{
    if (ShaderPrograms.Contains(InShaderProgramName))
    {
        return ShaderPrograms[InShaderProgramName]->GetVertexShaderName();
    }

    UE_LOG(LogLevel::Error, "Cannot Find ShaderProgram");
    
    return {""};
}

void FRenderer::MappingInputLayout(const FName InShaderProgramName, FName InInputLayoutName)
{
    if (ShaderPrograms.Contains(InShaderProgramName))
    {
        ShaderPrograms[InShaderProgramName] = std::make_shared<FShaderProgram>(ShaderPrograms[InShaderProgramName]->GetVertexShaderName(), ShaderPrograms[InShaderProgramName]->GetPixelShaderName(), InInputLayoutName);
    }
}

void FRenderer::MappingVSPSInputLayout(const FName InShaderProgramName, FName VSName, FName PSName, FName InInputLayoutName)
{
    ShaderPrograms.Add(InShaderProgramName, std::make_shared<FShaderProgram>(VSName, PSName, InInputLayoutName));
}

void FRenderer::MappingVSPSCBSlot(const FName InShaderName, const TMap<FShaderConstantKey, uint32>& MappedConstants)
{
    ShaderConstantNameAndSlots.Add(InShaderName, MappedConstants);
}

void FRenderer::MappingVBTopology(const FName InObjectName, const FName InVBName, const uint32 InStride, const uint32 InNumVertices, const D3D11_PRIMITIVE_TOPOLOGY InTopology)
{
    if (VBIBTopologyMappings.Contains(InObjectName) == false)
    {
        VBIBTopologyMappings[InObjectName] = std::make_shared<FVBIBTopologyMapping>();
    }
    VBIBTopologyMappings[InObjectName]->MappingVertexBuffer(InVBName, InStride, InNumVertices, InTopology);
}

void FRenderer::MappingIB(const FName InObjectName, const FName InIBName, const uint32 InNumIndices)
{
    if (VBIBTopologyMappings.Contains(InObjectName) == false)
    {
        VBIBTopologyMappings[InObjectName] = std::make_shared<FVBIBTopologyMapping>();
    }
    VBIBTopologyMappings[InObjectName]->MappingIndexBuffer(InIBName, InNumIndices);
}

size_t FRenderer::GetAtlasMemoryUsage(ELightType lightType) const
{
    return ShadowRenderPass->GetShadowMapAtlas(lightType)->GetEstimatedMemoryUsageInBytes(lightType);
}
