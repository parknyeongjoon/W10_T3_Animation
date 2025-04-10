#include "Renderer.h"
#include <d3dcompiler.h>

#include "Engine/World.h"
#include "Actors/Player.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/UBillboardComponent.h"
#include "Components/UParticleSubUVComp.h"
#include "Components/UText.h"
#include "Components/Material/Material.h"
#include "Components/HeightFogComponent.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Launch/EditorEngine.h"
#include "Math/JungleMath.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/Casts.h"
#include "UObject/Object.h"
#include "PropertyEditor/ShowFlags.h"
#include "UObject/UObjectIterator.h"
#include "Components/SkySphereComponent.h"
#include "D3D11RHI/FShaderProgram.h"
#include "Engine/UnrealClient.h"


void FRenderer::Initialize(FGraphicsDevice* graphics)
{
    Graphics = graphics;
    RenderResourceManager.Initialize(Graphics);
    CreateShader();
}

void FRenderer::Release()
{
    ReleaseShader();
}

#pragma region Shader
void FRenderer::CreateStaticMeshShader()
{
    ID3DBlob* VSBlob_StaticMesh = nullptr;
    ID3DBlob* PSBlob_StaticMesh = nullptr;

    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    ID3D11InputLayout* InputLayout;
    
    VertexShader = RenderResourceManager.GetVertexShader(TEXT("StaticMeshVS"));
    if (VertexShader == nullptr)
    {
        Graphics->CreateVertexShader(TEXT("StaticMeshVertexShader.hlsl"), nullptr, &VSBlob_StaticMesh, &VertexShader);
    }
    else
    {
        FGraphicsDevice::CompileVertexShader(TEXT("StaticMeshVertexShader.hlsl"), nullptr,  &VSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetVertexShader(TEXT("StaticMeshVS"), VertexShader);
    
    PixelShader = RenderResourceManager.GetPixelShader(TEXT("StaticMeshPS"));
    if (PixelShader == nullptr)
    {
        Graphics->CreatePixelShader(TEXT("StaticMeshPixelShader.hlsl"), nullptr, &PSBlob_StaticMesh, &PixelShader);
    }
    else
    {
        FGraphicsDevice::CompilePixelShader(TEXT("StaticMeshPixelShader.hlsl"), nullptr, &PSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetPixelShader(TEXT("StaticMeshPS"), PixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    Graphics->Device->CreateInputLayout(
        layoutDesc, ARRAYSIZE(layoutDesc), VSBlob_StaticMesh->GetBufferPointer(), VSBlob_StaticMesh->GetBufferSize(), &InputLayout
    );
    
    const TArray<FConstantBufferInfo> VertexStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(VSBlob_StaticMesh);
    const TArray<FConstantBufferInfo> PixelStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(PSBlob_StaticMesh);
    
    TMap<FShaderConstantKey, uint32> ShaderStageToCB;

    for (const FConstantBufferInfo item : VertexStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::VS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    for (const FConstantBufferInfo item :PixelStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::PS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    MappingVSPSInputLayout(TEXT("StaticMesh"), TEXT("StaticMeshVS"), TEXT("StaticMeshPS"), InputLayout);
    MappingVSPSCBSlot(TEXT("StaticMesh"), ShaderStageToCB);

    // TODO : Create RenderPass
    //staticMeshRenderPass = std::make_shared<StaticMeshRenderPass>(TEXT("StaticMesh"));
    //gizmoRenderPass = std::make_shared<GizmoRenderPass>(TEXT("StaticMesh"));

    SAFE_RELEASE(VSBlob_StaticMesh)
    SAFE_RELEASE(PSBlob_StaticMesh)
}

void FRenderer::CreateTextureShader()
{
    ID3DBlob* VSBlob_StaticMesh = nullptr;
    ID3DBlob* PSBlob_StaticMesh = nullptr;

    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    ID3D11InputLayout* InputLayout;
    
    VertexShader = RenderResourceManager.GetVertexShader(TEXT("TextureVS"));
    if (VertexShader == nullptr)
    {
        Graphics->CreateVertexShader(TEXT("TextureVertexShader.hlsl"), nullptr, &VSBlob_StaticMesh, &VertexShader);
    }
    else
    {
        FGraphicsDevice::CompileVertexShader(TEXT("TextureVertexShader.hlsl"), nullptr,  &VSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetVertexShader(TEXT("TextureVS"), VertexShader);
    
    PixelShader = RenderResourceManager.GetPixelShader(TEXT("TexturePS"));
    if (PixelShader == nullptr)
    {
        Graphics->CreatePixelShader(TEXT("TexturePixelShader.hlsl"), nullptr, &PSBlob_StaticMesh, &PixelShader);
    }
    else
    {
        FGraphicsDevice::CompilePixelShader(TEXT("TexturePixelShader.hlsl"), nullptr, &PSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetPixelShader(TEXT("TexturePS"), PixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    Graphics->Device->CreateInputLayout(
        layoutDesc, ARRAYSIZE(layoutDesc), VSBlob_StaticMesh->GetBufferPointer(), VSBlob_StaticMesh->GetBufferSize(), &InputLayout
    );
    
    const TArray<FConstantBufferInfo> VertexStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(VSBlob_StaticMesh);
    const TArray<FConstantBufferInfo> PixelStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(PSBlob_StaticMesh);
    
    TMap<FShaderConstantKey, uint32> ShaderStageToCB;

    for (const FConstantBufferInfo item : VertexStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::VS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    for (const FConstantBufferInfo item :PixelStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::PS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    MappingVSPSInputLayout(TEXT("Texture"), TEXT("TextureVS"), TEXT("TexturePS"), InputLayout);
    MappingVSPSCBSlot(TEXT("Texture"), ShaderStageToCB);

    // TODO : Create RenderPass

    SAFE_RELEASE(VSBlob_StaticMesh)
    SAFE_RELEASE(PSBlob_StaticMesh)
}

void FRenderer::CreateLineShader()
{
    ID3DBlob* VSBlob_StaticMesh = nullptr;
    ID3DBlob* PSBlob_StaticMesh = nullptr;

    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    ID3D11InputLayout* InputLayout;
    
    VertexShader = RenderResourceManager.GetVertexShader(TEXT("LineVS"));
    if (VertexShader == nullptr)
    {
        Graphics->CreateVertexShader(TEXT("LineVertexShader.hlsl"), nullptr, &VSBlob_StaticMesh, &VertexShader);
    }
    else
    {
        FGraphicsDevice::CompileVertexShader(TEXT("LineVertexShader.hlsl"), nullptr,  &VSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetVertexShader(TEXT("LineVS"), VertexShader);
    
    PixelShader = RenderResourceManager.GetPixelShader(TEXT("LinePS"));
    if (PixelShader == nullptr)
    {
        Graphics->CreatePixelShader(TEXT("LinePixelShader.hlsl"), nullptr, &PSBlob_StaticMesh, &PixelShader);
    }
    else
    {
        FGraphicsDevice::CompilePixelShader(TEXT("LinePixelShader.hlsl"), nullptr, &PSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetPixelShader(TEXT("LinePS"), PixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        // 정점 ID: 32비트 부호 없는 정수, 입력 슬로트 0, Per-Vertex 데이터
        { "SV_VertexID", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    
        // 인스턴스 ID: 32비트 부호 없는 정수, 입력 슬로트 1, Per-Instance 데이터
        { "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    
    Graphics->Device->CreateInputLayout(
        layoutDesc, ARRAYSIZE(layoutDesc), VSBlob_StaticMesh->GetBufferPointer(), VSBlob_StaticMesh->GetBufferSize(), &InputLayout
    );
    
    const TArray<FConstantBufferInfo> VertexStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(VSBlob_StaticMesh);
    const TArray<FConstantBufferInfo> PixelStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(PSBlob_StaticMesh);
    
    TMap<FShaderConstantKey, uint32> ShaderStageToCB;

    for (const FConstantBufferInfo item : VertexStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::VS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    for (const FConstantBufferInfo item :PixelStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::PS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    MappingVSPSInputLayout(TEXT("Line"), TEXT("LineVS"), TEXT("LinePS"), InputLayout);
    MappingVSPSCBSlot(TEXT("Line"), ShaderStageToCB);

    // TODO : Create RenderPass

    SAFE_RELEASE(VSBlob_StaticMesh)
    SAFE_RELEASE(PSBlob_StaticMesh)
}

void FRenderer::CreateFogShader()
{
    ID3DBlob* VSBlob_StaticMesh = nullptr;
    ID3DBlob* PSBlob_StaticMesh = nullptr;

    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    ID3D11InputLayout* InputLayout;
    
    VertexShader = RenderResourceManager.GetVertexShader(TEXT("FogVS"));
    if (VertexShader == nullptr)
    {
        Graphics->CreateVertexShader(TEXT("HeightFogVertexShader.hlsl"), nullptr, &VSBlob_StaticMesh, &VertexShader);
    }
    else
    {
        FGraphicsDevice::CompileVertexShader(TEXT("HeightFogVertexShader.hlsl"), nullptr,  &VSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetVertexShader(TEXT("FogVS"), VertexShader);
    
    PixelShader = RenderResourceManager.GetPixelShader(TEXT("FogPS"));
    if (PixelShader == nullptr)
    {
        Graphics->CreatePixelShader(TEXT("HeightFogPixelShader.hlsl"), nullptr, &PSBlob_StaticMesh, &PixelShader);
    }
    else
    {
        FGraphicsDevice::CompilePixelShader(TEXT("HeightFogPixelShader.hlsl"), nullptr, &PSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetPixelShader(TEXT("FogPS"), PixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        // 정점 ID: 32비트 부호 없는 정수, 입력 슬로트 0, Per-Vertex 데이터
        { "SV_VertexID", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    Graphics->Device->CreateInputLayout(
        layoutDesc, ARRAYSIZE(layoutDesc), VSBlob_StaticMesh->GetBufferPointer(), VSBlob_StaticMesh->GetBufferSize(), &InputLayout
    );
    
    const TArray<FConstantBufferInfo> VertexStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(VSBlob_StaticMesh);
    const TArray<FConstantBufferInfo> PixelStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(PSBlob_StaticMesh);
    
    TMap<FShaderConstantKey, uint32> ShaderStageToCB;

    for (const FConstantBufferInfo item : VertexStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::VS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    for (const FConstantBufferInfo item :PixelStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::PS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    MappingVSPSInputLayout(TEXT("Fog"), TEXT("FogVS"), TEXT("FogPS"), InputLayout);
    MappingVSPSCBSlot(TEXT("Fog"), ShaderStageToCB);

    // TODO : Create RenderPass

    SAFE_RELEASE(VSBlob_StaticMesh)
    SAFE_RELEASE(PSBlob_StaticMesh)
}

void FRenderer::CreateDebugDepthShader()
{
    ID3DBlob* VSBlob_StaticMesh = nullptr;
    ID3DBlob* PSBlob_StaticMesh = nullptr;

    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    ID3D11InputLayout* InputLayout;
    
    VertexShader = RenderResourceManager.GetVertexShader(TEXT("DebugDepthVS"));
    if (VertexShader == nullptr)
    {
        Graphics->CreateVertexShader(TEXT("DebugDepthVertexShader.hlsl"), nullptr, &VSBlob_StaticMesh, &VertexShader);
    }
    else
    {
        FGraphicsDevice::CompileVertexShader(TEXT("DebugDepthVertexShader.hlsl"), nullptr,  &VSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetVertexShader(TEXT("DebugDepthVS"), VertexShader);
    
    PixelShader = RenderResourceManager.GetPixelShader(TEXT("DebugDepthPS"));
    if (PixelShader == nullptr)
    {
        Graphics->CreatePixelShader(TEXT("DebugDepthPixelShader.hlsl"), nullptr, &PSBlob_StaticMesh, &PixelShader);
    }
    else
    {
        FGraphicsDevice::CompilePixelShader(TEXT("DebugDepthPixelShader.hlsl"), nullptr, &PSBlob_StaticMesh);
    }
    RenderResourceManager.AddOrSetPixelShader(TEXT("DebugDepthPS"), PixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        // 정점 ID: 32비트 부호 없는 정수, 입력 슬로트 0, Per-Vertex 데이터
        { "SV_VertexID", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    Graphics->Device->CreateInputLayout(
        layoutDesc, ARRAYSIZE(layoutDesc), VSBlob_StaticMesh->GetBufferPointer(), VSBlob_StaticMesh->GetBufferSize(), &InputLayout
    );
    
    const TArray<FConstantBufferInfo> VertexStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(VSBlob_StaticMesh);
    const TArray<FConstantBufferInfo> PixelStaticMeshConstant = FGraphicsDevice::ExtractConstantBufferNames(PSBlob_StaticMesh);
    
    TMap<FShaderConstantKey, uint32> ShaderStageToCB;

    for (const FConstantBufferInfo item : VertexStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::VS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    for (const FConstantBufferInfo item :PixelStaticMeshConstant)
    {
        ShaderStageToCB[{EShaderStage::PS, item.Name}] = item.BindSlot;
        if (RenderResourceManager.GetConstantBuffer(item.Name) == nullptr)
        {
            ID3D11Buffer* ConstantBuffer = RenderResourceManager.CreateConstantBuffer(item.ByteWidth);
            RenderResourceManager.AddOrSetConstantBuffer(item.Name, ConstantBuffer);
        }
    }

    MappingVSPSInputLayout(TEXT("DebugDepth"), TEXT("DebugDepthVS"), TEXT("DebugDepthPS"), InputLayout);
    MappingVSPSCBSlot(TEXT("DebugDepth"), ShaderStageToCB);

    // TODO : Create RenderPass

    SAFE_RELEASE(VSBlob_StaticMesh)
    SAFE_RELEASE(PSBlob_StaticMesh) 
}

void FRenderer::ReleaseShader()
{
    // RenderResourceManager.ReleaseShader(InputLayout, VertexShader, PixelShader);
    // ShaderManager.ReleaseShader(TextureInputLayout, VertexTextureShader, PixelTextureShader);
    // ShaderManager.ReleaseShader(nullptr, VertexLineShader, PixelLineShader);
    // ShaderManager.ReleaseShader(nullptr, DebugDepthVertexShader, DebugDepthPixelShader);
    // ShaderManager.ReleaseShader(nullptr, HeightFogVertexShader, HeightFogPixelShader);
}

void FRenderer::PrepareSubUVConstant() const
{
    if (SubUVConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(1, 1, &SubUVConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &SubUVConstantBuffer);
    }
}

void FRenderer::PrepareLineShader() const
{
    Graphics->DeviceContext->VSSetShader(VertexLineShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelLineShader, nullptr, 0);

    if (ConstantBuffer && GridConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);     // MatrixBuffer (b0)
        Graphics->DeviceContext->VSSetConstantBuffers(1, 1, &GridConstantBuffer); // GridParameters (b1)
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &GridConstantBuffer);
        Graphics->DeviceContext->VSSetConstantBuffers(3, 1, &LinePrimitiveBuffer);
        Graphics->DeviceContext->VSSetShaderResources(2, 1, &pBBSRV);
        Graphics->DeviceContext->VSSetShaderResources(3, 1, &pConeSRV);
        Graphics->DeviceContext->VSSetShaderResources(4, 1, &pOBBSRV);
    }
}
#pragma endregion Shader

#pragma endregion BlendState
void FRenderer::PrepareRender()
{
    if (GEngine->GetWorld()->WorldType == EWorldType::Editor)
    {
        for (const auto iter : TObjectRange<USceneComponent>())
        {
                UE_LOG(LogLevel::Display, "%d", GUObjectArray.GetObjectItemArrayUnsafe().Num());
                if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(iter))
                {
                    if (!Cast<UGizmoBaseComponent>(iter))
                        StaticMeshObjs.Add(pStaticMeshComp);
                }
                if (UGizmoBaseComponent* pGizmoComp = Cast<UGizmoBaseComponent>(iter))
                {
                    GizmoObjs.Add(pGizmoComp);
                }
                if (UBillboardComponent* pBillboardComp = Cast<UBillboardComponent>(iter))
                {
                    BillboardObjs.Add(pBillboardComp);
                }
                if (ULightComponentBase* pLightComp = Cast<ULightComponentBase>(iter))
                {
                    LightObjs.Add(pLightComp);
                }
        }
    }
    else if (GEngine->GetWorld()->WorldType == EWorldType::PIE)
    {
        for (const auto iter : GEngine->GetWorld()->GetActors())
        {
            
            for (const auto iter2 : iter->GetComponents())
            {
                if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(iter2))
                {
                    if (!Cast<UGizmoBaseComponent>(iter2))
                        StaticMeshObjs.Add(pStaticMeshComp);
                }
                if (UBillboardComponent* pBillboardComp = Cast<UBillboardComponent>(iter2))
                {
                    BillboardObjs.Add(pBillboardComp);
                }
                if (ULightComponentBase* pLightComp = Cast<ULightComponentBase>(iter2))
                {
                    LightObjs.Add(pLightComp);
                }
            }
        }
    }
}

void FRenderer::Render(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &ActiveViewport->GetD3DViewport());
    Graphics->ChangeRasterizer(ActiveViewport->GetViewMode());
    ChangeViewMode(ActiveViewport->GetViewMode());

    //RenderPostProcess(World, ActiveViewport);
    // 0. 광원 렌더
    RenderLight(World, ActiveViewport);

    // 1. 배치 렌더
    UPrimitiveBatch::GetInstance().RenderBatch(ConstantBuffer, ActiveViewport->GetViewMatrix(), ActiveViewport->GetProjectionMatrix());

    // 2. 스태틱 메시 렌더

    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))
        RenderStaticMeshes(World, ActiveViewport);

    // 3. 빌보드 렌더(빌보드, 텍스트)
    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))
        RenderBillboards(World, ActiveViewport);

    // 4. 기즈모 렌더
    RenderGizmos(World, ActiveViewport);


    // 6. 포스트 프로세스
    RenderPostProcess(World, ActiveViewport, ActiveViewport);

    ClearRenderArr();
}

void FRenderer::Render(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport, std::shared_ptr<FEditorViewportClient> CurrentViewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &ActiveViewport->GetD3DViewport());
    Graphics->ChangeRasterizer(ActiveViewport->GetViewMode());
    ChangeViewMode(ActiveViewport->GetViewMode());

    //RenderPostProcess(World, ActiveViewport);
    // 0. 광원 렌더
    RenderLight(World, ActiveViewport);

    // 1. 배치 렌더
    UPrimitiveBatch::GetInstance().RenderBatch(ConstantBuffer, ActiveViewport->GetViewMatrix(), ActiveViewport->GetProjectionMatrix());

    // 2. 스태틱 메시 렌더

    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))
        RenderStaticMeshes(World, ActiveViewport);

    // 3. 빌보드 렌더(빌보드, 텍스트)
    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))
        RenderBillboards(World, ActiveViewport);

    // 4. 기즈모 렌더
    RenderGizmos(World, ActiveViewport);


    // 6. 포스트 프로세스
    RenderPostProcess(World, ActiveViewport, CurrentViewport);

    ClearRenderArr();
}

void FRenderer::RenderPrimitive(OBJ::FStaticMeshRenderData* renderData, TArray<FStaticMaterial*> materials, TArray<UMaterial*> overrideMaterial, int selectedSubMeshIndex = -1) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &renderData->VertexBuffer, &Stride, &offset);

    if (renderData->IndexBuffer)
        Graphics->DeviceContext->IASetIndexBuffer(renderData->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    if (renderData->MaterialSubsets.Num() == 0)
    {
        // no submesh
        Graphics->DeviceContext->DrawIndexed(renderData->Indices.Num(), 0, 0);
    }

    for (int subMeshIndex = 0; subMeshIndex < renderData->MaterialSubsets.Num(); subMeshIndex++)
    {
        int materialIndex = renderData->MaterialSubsets[subMeshIndex].MaterialIndex;

        subMeshIndex == selectedSubMeshIndex ? ConstantBufferUpdater.UpdateSubMeshConstant(SubMeshConstantBuffer, true) : ConstantBufferUpdater.UpdateSubMeshConstant(SubMeshConstantBuffer, false);

        overrideMaterial[materialIndex] != nullptr ?
            UpdateMaterial(overrideMaterial[materialIndex]->GetMaterialInfo()) : UpdateMaterial(materials[materialIndex]->Material->GetMaterialInfo());

        if (renderData->IndexBuffer)
        {
            // index draw
            uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
            uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
            Graphics->DeviceContext->DrawIndexed(indexCount, startIndex, 0);
        }
    }
}

void FRenderer::RenderTexturePrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices, ID3D11ShaderResourceView* _TextureSRV,
    ID3D11SamplerState* _SamplerState
) const
{
    if (!_TextureSRV || !_SamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    if (numIndices <= 0)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "numIndices Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &TextureStride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &_TextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &_SamplerState);
    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FRenderer::RenderTextPrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11ShaderResourceView* _TextureSRV, ID3D11SamplerState* _SamplerState
) const
{
    if (!_TextureSRV || !_SamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &TextureStride, &offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &_TextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &_SamplerState);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FRenderer::RenderStaticMeshes(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    PrepareShader();
    for (UStaticMeshComponent* StaticMeshComp : StaticMeshObjs)
    {
        FMatrix Model = JungleMath::CreateModelMatrix(
            StaticMeshComp->GetWorldLocation(),
            StaticMeshComp->GetWorldRotation(),
            StaticMeshComp->GetWorldScale()
        );
        // 최종 MVP 행렬
		FMatrix ViewProj = ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();
        // 노말 회전시 필요 행렬
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = StaticMeshComp->EncodeUUID() / 255.0f;
        if (World->GetSelectedActor() == StaticMeshComp->GetOwner())
        {
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, true);
        }
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, false);

        if (USkySphereComponent* skysphere = Cast<USkySphereComponent>(StaticMeshComp))
        {
            ConstantBufferUpdater.UpdateTextureConstant(TextureConstantBufer, skysphere->UOffset, skysphere->VOffset);
        }
        else
        {
            ConstantBufferUpdater.UpdateTextureConstant(TextureConstantBufer, 0, 0);
        }

        if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_AABB))
        {
            UPrimitiveBatch::GetInstance().RenderAABB(
                StaticMeshComp->GetBoundingBox(),
                StaticMeshComp->GetWorldLocation(),
                Model
            );
        }


        if (!StaticMeshComp->GetStaticMesh()) continue;

        OBJ::FStaticMeshRenderData* renderData = StaticMeshComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        RenderPrimitive(renderData, StaticMeshComp->GetStaticMesh()->GetMaterials(), StaticMeshComp->GetOverrideMaterials(), StaticMeshComp->GetselectedSubMeshIndex());
    }
}

void FRenderer::RenderGizmos(const UWorld* World, const std::shared_ptr<FEditorViewportClient>& ActiveViewport)
{
    if (!World->GetSelectedActor())
    {
        return;
    }

#pragma region GizmoDepth
    ID3D11DepthStencilState* DepthStateDisable = Graphics->DepthStateDisable;
    Graphics->DeviceContext->OMSetDepthStencilState(DepthStateDisable, 0);
#pragma endregion GizmoDepth

    //  fill solid,  Wirframe 에서도 제대로 렌더링되기 위함
    Graphics->DeviceContext->RSSetState(UEditorEngine::graphicDevice.RasterizerStateSOLID);

    for (auto GizmoComp : GizmoObjs)
    {

        if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_TRANSLATION)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_SCALE)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_ROTATION)
            continue;
        FMatrix Model = JungleMath::CreateModelMatrix(GizmoComp->GetWorldLocation(),
            GizmoComp->GetWorldRotation(),
            GizmoComp->GetWorldScale()
        );
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = GizmoComp->EncodeUUID() / 255.0f;

		FMatrix ViewProj = ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();
        if (GizmoComp == World->GetPickingGizmo())
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, true);
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, false);

        if (!GizmoComp->GetStaticMesh()) continue;

        OBJ::FStaticMeshRenderData* renderData = GizmoComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        RenderPrimitive(renderData, GizmoComp->GetStaticMesh()->GetMaterials(), GizmoComp->GetOverrideMaterials());
    }

    Graphics->DeviceContext->RSSetState(Graphics->GetCurrentRasterizer());

#pragma region GizmoDepth
    ID3D11DepthStencilState* originalDepthState = Graphics->DepthStencilState;
    Graphics->DeviceContext->OMSetDepthStencilState(originalDepthState, 0);
#pragma endregion GizmoDepth
}

void FRenderer::RenderBillboards(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    PrepareTextureShader();
    PrepareSubUVConstant();
    for (auto BillboardComp : BillboardObjs)
    {
        ConstantBufferUpdater.UpdateSubUVConstant(SubUVConstantBuffer, BillboardComp->finalIndexU, BillboardComp->finalIndexV);

        FMatrix Model = BillboardComp->CreateBillboardMatrix();

        // 최종 MVP 행렬
		FMatrix ViewProj = ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();

        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = BillboardComp->EncodeUUID() / 255.0f;
        if (BillboardComp == World->GetPickingGizmo())
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, true);
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, false);

        if (UParticleSubUVComp* SubUVParticle = Cast<UParticleSubUVComp>(BillboardComp))
        {
            RenderTexturePrimitive(
                SubUVParticle->vertexSubUVBuffer, SubUVParticle->numTextVertices,
                SubUVParticle->indexTextureBuffer, SubUVParticle->numIndices, SubUVParticle->Texture->TextureSRV, SubUVParticle->Texture->SamplerState
            );
        }
        else if (UText* Text = Cast<UText>(BillboardComp))
        {
            UEditorEngine::renderer.RenderTextPrimitive(
                Text->vertexTextBuffer, Text->numTextVertices,
                Text->Texture->TextureSRV, Text->Texture->SamplerState
            );
        }
        else
        {
            RenderTexturePrimitive(
                BillboardComp->vertexTextureBuffer, BillboardComp->numVertices,
                BillboardComp->indexTextureBuffer, BillboardComp->numIndices, BillboardComp->Texture->TextureSRV, BillboardComp->Texture->SamplerState
            );
        }
    }
    PrepareShader();
}

void FRenderer::RenderPostProcess(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport, std::shared_ptr<FEditorViewportClient> CurrentViewport)
{

    RenderDebugDepth(ActiveViewport);
    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Fog)) 
    {
        RenderHeightFog(ActiveViewport, CurrentViewport);
    }
}

void FRenderer::RenderDebugDepth(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    // 현재 뷰포트의 뷰모드가 Depth 인지 확인
    if (ActiveViewport->GetViewMode() != EViewModeIndex::VMI_Depth)
    {
        return;
    }

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Graphics->DeviceContext->CopyResource(Graphics->DepthCopyTexture, Graphics->DepthStencilBuffer);

    
    Graphics->DeviceContext->PSSetSamplers(0, 1, &DebugDepthSRVSampler);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &Graphics->DepthCopySRV);

    Graphics->DeviceContext->VSSetShader(DebugDepthVertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(DebugDepthPixelShader, nullptr, 0);
    
    FViewportConstants ViewportConstants;
    ViewportConstants.ViewportWidth = ActiveViewport->Viewport->GetViewport().Width / Graphics->screenWidth;
    ViewportConstants.ViewportHeight = ActiveViewport->Viewport->GetViewport().Height/ Graphics->screenHeight;
    ViewportConstants.ViewportOffsetX = ActiveViewport->Viewport->GetViewport().TopLeftX/ Graphics->screenWidth;
    ViewportConstants.ViewportOffsetY = ActiveViewport->Viewport->GetViewport().TopLeftY/ Graphics->screenHeight;
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(ViewportConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &ViewportConstants, sizeof(FViewportConstants));
    Graphics->DeviceContext->Unmap(ViewportConstantBuffer, 0);

    // 렌더링 시 샘플러 설정
    Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &CameraConstantBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &CameraConstantBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &ViewportConstantBuffer);

    Graphics->DeviceContext->Draw(4, 0);
}

void FRenderer::RenderLight(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    // 라이트 오브젝트들 모아서 Constant 업데이트 해야함
    ConstantBufferUpdater.UpdateLightConstant(LightingBuffer, LightObjs);
}

void FRenderer::RenderBatch(
    const FGridParameters& gridParam, ID3D11Buffer* pVertexBuffer, int boundingBoxCount, int coneCount, int coneSegmentCount, int obbCount
) const
{
    UINT stride = sizeof(FSimpleVertex);
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    UINT vertexCountPerInstance = 2;
    UINT instanceCount = gridParam.numGridLines + 3 + (boundingBoxCount * 12) + (coneCount * (2 * coneSegmentCount)) + (12 * obbCount);
    Graphics->DeviceContext->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#pragma endregion Render

void FRenderer::ChangeViewMode(EViewModeIndex evi) const
{
    switch (evi)
    {
    case EViewModeIndex::VMI_Lit:
        ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 1);
        break;
    case EViewModeIndex::VMI_Wireframe:
    case EViewModeIndex::VMI_Unlit:
        ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 0);
        break;
    }
}

void FRenderer::UpdateMaterial(const FObjMaterialInfo& MaterialInfo) const
{
    ConstantBufferUpdater.UpdateMaterialConstant(MaterialConstantBuffer, MaterialInfo);

    if (MaterialInfo.bHasTexture == true)
    {
        std::shared_ptr<FTexture> texture = UEditorEngine::resourceMgr.GetTexture(MaterialInfo.DiffuseTexturePath);
        if (texture)
        {
            Graphics->DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
            Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
        }
    }
    else
    {
        ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
        ID3D11SamplerState* nullSampler[1] = {nullptr};

        Graphics->DeviceContext->PSSetShaderResources(0, 1, nullSRV);
        Graphics->DeviceContext->PSSetSamplers(0, 1, nullSampler);
    }
}



ID3D11ShaderResourceView* FRenderer::CreateBoundingBoxSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numBoundingBoxes;


    Graphics->Device->CreateShaderResourceView(pBoundingBoxBuffer, &srvDesc, &pBBSRV);
    return pBBSRV;
}

ID3D11ShaderResourceView* FRenderer::CreateOBBSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numBoundingBoxes;
    Graphics->Device->CreateShaderResourceView(pBoundingBoxBuffer, &srvDesc, &pOBBSRV);
    return pOBBSRV;
}

ID3D11ShaderResourceView* FRenderer::CreateConeSRV(ID3D11Buffer* pConeBuffer, UINT numCones)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; 
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numCones;


    Graphics->Device->CreateShaderResourceView(pConeBuffer, &srvDesc, &pConeSRV);
    return pConeSRV;
}

void FRenderer::UpdateBoundingBoxBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FBoundingBox>& BoundingBoxes, int numBoundingBoxes) const
{
    if (!pBoundingBoxBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pBoundingBoxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FBoundingBox*>(mappedResource.pData);
    for (int i = 0; i < BoundingBoxes.Num(); ++i)
    {
        pData[i] = BoundingBoxes[i];
    }
    Graphics->DeviceContext->Unmap(pBoundingBoxBuffer, 0);
}

void FRenderer::UpdateOBBBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FOBB>& BoundingBoxes, int numBoundingBoxes) const
{
    if (!pBoundingBoxBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pBoundingBoxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FOBB*>(mappedResource.pData);
    for (int i = 0; i < BoundingBoxes.Num(); ++i)
    {
        pData[i] = BoundingBoxes[i];
    }
    Graphics->DeviceContext->Unmap(pBoundingBoxBuffer, 0);
}

void FRenderer::UpdateConesBuffer(ID3D11Buffer* pConeBuffer, const TArray<FCone>& Cones, int numCones) const
{
    if (!pConeBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pConeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FCone*>(mappedResource.pData);
    for (int i = 0; i < Cones.Num(); ++i)
    {
        pData[i] = Cones[i];
    }
    Graphics->DeviceContext->Unmap(pConeBuffer, 0);
}

void FRenderer::UpdateGridConstantBuffer(const FGridParameters& gridParams) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = Graphics->DeviceContext->Map(GridConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &gridParams, sizeof(FGridParameters));
        Graphics->DeviceContext->Unmap(GridConstantBuffer, 0);
    }
    else
    {
        UE_LOG(LogLevel::Warning, "gridParams");
    }
}

void FRenderer::UpdateLinePrimitveCountBuffer(int numBoundingBoxes, int numCones) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = Graphics->DeviceContext->Map(LinePrimitiveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = static_cast<FPrimitiveCounts*>(mappedResource.pData);
    pData->BoundingBoxCount = numBoundingBoxes;
    pData->ConeCount = numCones;
    Graphics->DeviceContext->Unmap(LinePrimitiveBuffer, 0);
}

void FRenderer::RenderHeightFog(std::shared_ptr<FEditorViewportClient> ActiveViewport, std::shared_ptr<FEditorViewportClient> CurrentViewport)
{
    // 활성화된 Height Fog 컴포넌트 찾기
    UHeightFogComponent* HeightFogComp = nullptr;
    for (const auto& comp: TObjectRange<UHeightFogComponent>() )
    {
        HeightFogComp = comp;
    }

    if (!HeightFogComp) return;
    if (!HeightFogComp->bIsActive) return;

    Graphics->DeviceContext->VSSetShader(HeightFogVertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(HeightFogPixelShader, nullptr, 0);

    Graphics->DeviceContext->PSSetSamplers(0, 1, &DebugDepthSRVSampler);

    // Fog Constant buffer update
    FHeightFogConstants fogParams;
    fogParams.FogDensity = HeightFogComp->FogDensity;
    fogParams.HeightFogStart = HeightFogComp->HeightFogStart;
    fogParams.HeightFogEnd = HeightFogComp->HeightFogEnd;
    fogParams.DistanceFogNear = HeightFogComp->DistanceFogNear;
    fogParams.DistanceFogFar = HeightFogComp->DistanceFogFar;
    fogParams.MaxOpacity = HeightFogComp->FogMaxOpacity;
    fogParams.InscatteringColor = FLinearColor(
        HeightFogComp->FogInscatteringColor.R,
        HeightFogComp->FogInscatteringColor.G,
        HeightFogComp->FogInscatteringColor.B,
        HeightFogComp->FogInscatteringColor.A
    );
    fogParams.DirectionalInscatteringColor = FLinearColor(
        HeightFogComp->DirectionalInscatteringColor.R,
        HeightFogComp->DirectionalInscatteringColor.G,
        HeightFogComp->DirectionalInscatteringColor.B,
        HeightFogComp->DirectionalInscatteringColor.A
    );
    // 현재 0, 0, -1 방향의 가상의 Directional light로 고정함
    // Unreal의 경우 Default Directional light(Sun)의 값을 따라가는 것으로 보임
    // 실제 Directional light를 가져오도록 할 지 고려, 또한 Directional light이 2개 이상일 때는 어떻게 처리할지?
    fogParams.DirectionalLightDirection = HeightFogComp->DirectionalLightDirection;
    fogParams.DirectionalInscatteringExponent = HeightFogComp->DirectionalInscatteringExponent;
    fogParams.DirectionalInscatteringStartDistance = HeightFogComp->DirectionalInscatteringStartDistance;
    fogParams.IsExponential = HeightFogComp->bIsExponential ? 1 : 0;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(FogConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &fogParams, sizeof(FHeightFogConstants));
    Graphics->DeviceContext->Unmap(FogConstantBuffer, 0);

    FViewportConstants ViewportConstants;
    ViewportConstants.ViewportWidth = CurrentViewport->Viewport->GetViewport().Width / Graphics->screenWidth;
    ViewportConstants.ViewportHeight = CurrentViewport->Viewport->GetViewport().Height / Graphics->screenHeight;
    ViewportConstants.ViewportOffsetX = CurrentViewport->Viewport->GetViewport().TopLeftX / Graphics->screenWidth;
    ViewportConstants.ViewportOffsetY = CurrentViewport->Viewport->GetViewport().TopLeftY / Graphics->screenHeight;

    D3D11_MAPPED_SUBRESOURCE viewportMappedResource;
    Graphics->DeviceContext->Map(ViewportConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &viewportMappedResource);
    memcpy(viewportMappedResource.pData, &ViewportConstants, sizeof(FViewportConstants));
    Graphics->DeviceContext->Unmap(ViewportConstantBuffer, 0);

    // 백퍼버 복사
    Graphics->CreateSceneColorResources();
    Graphics->DeviceContext->CopyResource(Graphics->DepthCopyTexture, Graphics->DepthStencilBuffer);
    // 겹치지 않도록 하기 위해 임시로 Slot를 현재 사용하지 않는 슬롯으로 지정함
    Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &CameraConstantBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(6, 1, &FogConstantBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &ViewportConstantBuffer);
    Graphics->DeviceContext->PSSetShaderResources(5, 1, &Graphics->SceneColorSRV);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &Graphics->DepthCopySRV);

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Graphics->DeviceContext->Draw(4, 0);
}

void FRenderer::MappingVSPSInputLayout(const FName InShaderProgramName, FName VSName, FName PSName, ID3D11InputLayout* InputLayout)
{
    ShaderPrograms.Add(InShaderProgramName, std::make_shared<FShaderProgram>(VSName, PSName, InputLayout));
}

void FRenderer::MappingVSPSCBSlot(const FName InShaderName, const TMap<FShaderConstantKey, uint32>& MappedConstants)
{
    ShaderConstantNameAndSlots.Add(InShaderName, MappedConstants);
}
