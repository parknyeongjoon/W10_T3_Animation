#include "RenderResourceManager.h"

#include <d3dcompiler.h>

#include <memory>

#include "D3D11RHI/PixelShader.h"
#include "D3D11RHI/VertexShader.h"

FRenderResourceManager::FRenderResourceManager(FGraphicsDevice* InGraphicDevice)
{
    GraphicDevice = InGraphicDevice;
}

void FRenderResourceManager::Initialize()
{
    LoadStates();
}

void FRenderResourceManager::LoadStates()
{
#pragma region sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	GraphicDevice->Device->CreateSamplerState(&samplerDesc, &SamplerStates[static_cast<uint32>(ESamplerType::Anisotropic)]);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    GraphicDevice->Device->CreateSamplerState(&samplerDesc, &SamplerStates[static_cast<uint32>(ESamplerType::Point)]);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    GraphicDevice->Device->CreateSamplerState(&samplerDesc, &SamplerStates[static_cast<uint32>(ESamplerType::Linear)]);

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    GraphicDevice->Device->CreateSamplerState(&samplerDesc, &SamplerStates[static_cast<uint32>(ESamplerType::PostProcess)]);

    // 비교 샘플러 상태 생성
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; // 비교 필터링 모드
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; // 그림자 맵 비교에 적합한 LESS
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    GraphicDevice->Device->CreateSamplerState(&samplerDesc, &SamplerStates[static_cast<uint32>(ESamplerType::ComparisonSampler)]);

	GraphicDevice->BindSamplers(static_cast<uint32>(ESamplerType::Point), 1, &SamplerStates[static_cast<uint32>(ESamplerType::Point)]);
	GraphicDevice->BindSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &SamplerStates[static_cast<uint32>(ESamplerType::Linear)]);
	GraphicDevice->BindSamplers(static_cast<uint32>(ESamplerType::Anisotropic), 1, &SamplerStates[static_cast<uint32>(ESamplerType::Anisotropic)]);
	GraphicDevice->BindSamplers(static_cast<uint32>(ESamplerType::PostProcess), 1, &SamplerStates[static_cast<uint32>(ESamplerType::PostProcess)]);
    GraphicDevice->BindSamplers(static_cast<uint32>(ESamplerType::ComparisonSampler), 1, &SamplerStates[static_cast<uint32>(ESamplerType::ComparisonSampler)]);

#pragma endregion
#pragma region rasterize state
    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.AntialiasedLineEnable = false;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0.0f;
    rsDesc.DepthClipEnable = true;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.FrontCounterClockwise = false;
    rsDesc.MultisampleEnable = false;
    rsDesc.ScissorEnable = false;
    rsDesc.SlopeScaledDepthBias = 0.0f;
    GraphicDevice->CreateRasterizerState(
        &rsDesc, &RasterizerStates[static_cast<uint32>(ERasterizerState::SolidBack)]);

    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_FRONT;
    GraphicDevice->CreateRasterizerState(
        &rsDesc, &RasterizerStates[static_cast<uint32>(ERasterizerState::SolidFront)]);

    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_NONE;
    GraphicDevice->CreateRasterizerState(
        &rsDesc, &RasterizerStates[static_cast<uint32>(ERasterizerState::SolidNone)]);

    rsDesc.FillMode = D3D11_FILL_WIREFRAME;
    rsDesc.CullMode = D3D11_CULL_NONE;
    GraphicDevice->CreateRasterizerState(
        &rsDesc, &RasterizerStates[static_cast<uint32>(ERasterizerState::WireFrame)]);
#pragma endregion
#pragma region blend state
    D3D11_BLEND_DESC bsDesc = {};
    bsDesc.AlphaToCoverageEnable = false;
    bsDesc.IndependentBlendEnable = false;
    bsDesc.RenderTarget[0].BlendEnable = true;
    bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    GraphicDevice->CreateBlendState(&bsDesc, &BlendStates[static_cast<uint32>(EBlendState::AlphaBlend)]);

    bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    GraphicDevice->CreateBlendState(&bsDesc, &BlendStates[static_cast<uint32>(EBlendState::OneOne)]);
#pragma endregion
#pragma region depthstencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    GraphicDevice->CreateDepthStencilState(
        &dsDesc, &DepthStencilStates[static_cast<uint32>(EDepthStencilState::LessEqual)]);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = FALSE;  // 깊이 테스트 유지
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // 깊이 버퍼에 쓰지 않음
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;  // 깊이 비교를 항상 통과
    GraphicDevice->CreateDepthStencilState(
        &depthStencilDesc, &DepthStencilStates[static_cast<uint32>(EDepthStencilState::DepthNone)]);
#pragma endregion
}

void FRenderResourceManager::ReleaseResources()
{
    for (ID3D11DepthStencilState* depthStencilState : DepthStencilStates)
    {
        depthStencilState->Release();
        depthStencilState = nullptr;
    }

    for (ID3D11RasterizerState* rasterizerState : RasterizerStates)
    {
        rasterizerState->Release();
        rasterizerState = nullptr;
    }

    for (ID3D11BlendState* blendState : BlendStates)
    {
        blendState->Release();
        blendState = nullptr;
    }

    for (ID3D11SamplerState* samplerState : SamplerStates)
    {
        samplerState->Release();
        samplerState = nullptr;
    }

    for (auto CB : ConstantBuffers)
    {
        CB.Value->Release();
        CB.Value = nullptr;
    }

    for (auto IB : IndexBuffers)
    {
        IB.Value->Release();
        IB.Value = nullptr;
    }

    for (auto VB : VertexBuffers)
    {
        VB.Value->Release();
        VB.Value = nullptr;
    }
}

ID3D11Buffer* FRenderResourceManager::CreateIndexBuffer(const uint32* indices, const uint32 indicesSize) const
{
    TArray<uint32> indicesToCopy;
    indicesToCopy.AppendArray(indices, indicesSize);

    return CreateIndexBuffer(indicesToCopy);
}

ID3D11Buffer* FRenderResourceManager::CreateIndexBuffer(const TArray<uint32>& indices) const
{
    D3D11_BUFFER_DESC indexbufferdesc = {};              
    indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;       
    indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexbufferdesc.ByteWidth = sizeof(uint32) * indices.Num();

    D3D11_SUBRESOURCE_DATA indexbufferSRD;
    indexbufferSRD.pSysMem = indices.GetData();

    ID3D11Buffer* indexBuffer;

    const HRESULT hr = GraphicDevice->Device->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &indexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "IndexBuffer Creation faild");
    }
    return indexBuffer;
}

ID3D11Buffer* FRenderResourceManager::CreateConstantBuffer(const uint32 InSize, const void* InData) const
{
    D3D11_BUFFER_DESC constantBufferDesc = {};   
    constantBufferDesc.ByteWidth = InSize;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA sub = {};
    sub.pSysMem = InData;
    
    ID3D11Buffer* constantBuffer;

    HRESULT hr;
    if (InData == nullptr)
        hr = GraphicDevice->Device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
    else
        hr = GraphicDevice->Device->CreateBuffer(&constantBufferDesc, &sub, &constantBuffer);

    if (FAILED(hr))
        assert(NULL/*"Create constant buffer failed!"*/);

    return constantBuffer;
}

ID3D11ShaderResourceView* FRenderResourceManager::CreateBufferSRV(ID3D11Buffer* pBuffer, UINT numElements) const
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 구조화된 버퍼의 경우 형식은 UNKNOWN으로 지정
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numElements;

    ID3D11ShaderResourceView* pSRV = nullptr;
    const HRESULT hr = GraphicDevice->Device->CreateShaderResourceView(pBuffer, &srvDesc, &pSRV);
    if (FAILED(hr))
    {
        // 오류 처리 (필요에 따라 로그 출력 등)
        assert(false && "CreateStructuredBufferShaderResourceView failed");
        return nullptr;
    }
    return pSRV;
}

ID3D11UnorderedAccessView* FRenderResourceManager::CreateBufferUAV(ID3D11Buffer* pBuffer, UINT numElements) const
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 구조화된 버퍼의 경우 형식은 UNKNOWN으로 지정
    srvDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = numElements;

    ID3D11UnorderedAccessView* pSRV = nullptr;
    const HRESULT hr = GraphicDevice->Device->CreateUnorderedAccessView(pBuffer, &srvDesc, &pSRV);
    if (FAILED(hr))
    {
        // 오류 처리 (필요에 따라 로그 출력 등)
        assert(false && "CreateStructuredBufferShaderResourceView failed");
        return nullptr;
    }
    return pSRV;
}

void FRenderResourceManager::CreateVertexShader(const FString& InShaderName, const FString& InFileName, D3D_SHADER_MACRO* pDefines)
{
    ID3DBlob* VSBlob = nullptr;

    const std::filesystem::path current = std::filesystem::current_path();
    const std::filesystem::path fullpath = current / TEXT("Shaders") / *InFileName;

    ID3D11VertexShader* VertexShader = GetVertexShader(InShaderName);
    if (VertexShader == nullptr)
    {
        GraphicDevice->CreateVertexShader(fullpath, pDefines, &VSBlob, &VertexShader);
    }
    
    std::filesystem::file_time_type CurrentVSWriteTime = std::filesystem::last_write_time(fullpath);
#if USE_WIDECHAR
    AddOrSetVertexShader(InShaderName, fullpath.wstring(), VertexShader, VSBlob, pDefines, CurrentVSWriteTime);
#else
    AddOrSetVertexShader(InShaderName, fullpath.string(), VertexShader, VSBlob, pDefines, CurrentVSWriteTime);
#endif
}

void FRenderResourceManager::UpdateVertexShader(const FString& InShaderName, const FString& InFullPath, D3D_SHADER_MACRO* pDefines)
{
    ID3DBlob* VSBlob = nullptr;

    const std::filesystem::path current = std::filesystem::current_path();
    const std::filesystem::path fullpath = *InFullPath;

    ID3D11VertexShader* VertexShader;
    GraphicDevice->CreateVertexShader(fullpath, pDefines, &VSBlob, &VertexShader);
    
    std::filesystem::file_time_type CurrentVSWriteTime = std::filesystem::last_write_time(fullpath);
#if USE_WIDECHAR
    AddOrSetVertexShader(InShaderName, fullpath.wstring(), VertexShader, VSBlob, pDefines, CurrentVSWriteTime);
#else
    AddOrSetVertexShader(InShaderName, fullpath.string(), VertexShader, VSBlob, pDefines, CurrentVSWriteTime);
#endif
}

void FRenderResourceManager::CreatePixelShader(const FString& InShaderName, const FString& InFileName, D3D_SHADER_MACRO* pDefines)
{
    ID3DBlob* PSBlob = nullptr;
    
    const std::filesystem::path current = std::filesystem::current_path();
    const std::filesystem::path fullpath = current / TEXT("Shaders") / *InFileName;

    ID3D11PixelShader* PixelShader = GetPixelShader(InShaderName);
    if (PixelShader == nullptr)
    {
        GraphicDevice->CreatePixelShader(fullpath, pDefines, &PSBlob, &PixelShader);
    }

    std::filesystem::file_time_type CurrentPSWriteTime = std::filesystem::last_write_time(fullpath);
#if USE_WIDECHAR
    AddOrSetPixelShader(InShaderName, fullpath.wstring(), PixelShader, PSBlob, pDefines, CurrentPSWriteTime);
#else
    AddOrSetPixelShader(InShaderName, fullpath.string(), PixelShader, PSBlob, pDefines, CurrentPSWriteTime);
#endif
}

void FRenderResourceManager::UpdatePixelShader(const FString& InShaderName, const FString& InFullPath, D3D_SHADER_MACRO* pDefines)
{
    ID3DBlob* PSBlob = nullptr;
    
    const std::filesystem::path fullpath = *InFullPath;

    ID3D11PixelShader* PixelShader;
    if (pDefines != nullptr)
    {
        GraphicDevice->CreatePixelShader(fullpath, pDefines, &PSBlob, &PixelShader);
    }
    else
    {
        GraphicDevice->CreatePixelShader(fullpath.string(), nullptr, &PSBlob, &PixelShader);
    }

    std::filesystem::file_time_type CurrentPSWriteTime = std::filesystem::last_write_time(fullpath);
#if USE_WIDECHAR
    AddOrSetPixelShader(InShaderName, fullpath.wstring(), PixelShader, PSBlob, pDefines, CurrentPSWriteTime);
#else
    AddOrSetPixelShader(InShaderName, fullpath.string(), PixelShader, PSBlob, pDefines, CurrentPSWriteTime);
#endif
}

void FRenderResourceManager::AddOrSetVertexShader(const FName InVSName, const FString& InFullPath, ID3D11VertexShader* InVS, ID3DBlob* InShaderBlob,
                                                  D3D_SHADER_MACRO* InShaderMacro, std::filesystem::file_time_type InWriteTime)
{
    if (VertexShaders.Contains(InVSName))
    {
        VertexShaders[InVSName]->Release();
    }
    VertexShaders.Add(InVSName, std::make_shared<FVertexShader>(InVSName, InFullPath, InVS, InShaderBlob, InShaderMacro, InWriteTime));
}

void FRenderResourceManager::AddOrSetPixelShader(FName InPSName, const FString& InFullPath, ID3D11PixelShader* InPS, ID3DBlob* InShaderBlob,
                                                 D3D_SHADER_MACRO* InShaderMacro, std::filesystem::file_time_type InWriteTime)
{
    if (PixelShaders.Contains(InPSName))
    {
        PixelShaders[InPSName]->Release();
    }
    PixelShaders.Add(InPSName, std::make_shared<FPixelShader>(InPSName, InFullPath, InPS, InShaderBlob, InShaderMacro, InWriteTime));
}

void FRenderResourceManager::AddOrSetInputLayout(const FName InInputLayoutName, ID3D11InputLayout* InInputLayout)
{
    if (InputLayouts.Contains(InInputLayoutName))
    {
        InputLayouts[InInputLayoutName]->Release();
    }
    InputLayouts.Add(InInputLayoutName, InInputLayout);
}

void FRenderResourceManager::AddOrSetComputeShader(const FName InCSName, ID3D11ComputeShader* InShader)
{
    if (ComputeShaders.Contains(InCSName))
    {
        ComputeShaders[InCSName]->Release();
    }
    ComputeShaders.Add(InCSName, InShader);
}

void FRenderResourceManager::AddOrSetVertexBuffer(const FName InVBName, ID3D11Buffer* InBuffer)
{
    if (VertexBuffers.Contains(InVBName))
    {
        VertexBuffers[InVBName]->Release();
    }
    VertexBuffers.Add(InVBName, InBuffer);
}

void FRenderResourceManager::AddOrSetIndexBuffer(const FName InPBName, ID3D11Buffer* InBuffer)
{
    if (IndexBuffers.Contains(InPBName))
    {
        IndexBuffers[InPBName]->Release();
    }
    IndexBuffers.Add(InPBName, InBuffer);
}

void FRenderResourceManager::AddOrSetConstantBuffer(const FName InCBName, ID3D11Buffer* InBuffer)
{
    if (ConstantBuffers.Contains(InCBName))
    {
        ConstantBuffers[InCBName]->Release();
    }
    ConstantBuffers.Add(InCBName, InBuffer);
}

void FRenderResourceManager::AddOrSetSRVStructuredBuffer(const FName InSBName, ID3D11Buffer* InBuffer)
{
    if (SRVStructuredBuffers.Contains(InSBName) == false)
    {
        SRVStructuredBuffers[InSBName] = TPair<ID3D11Buffer*, ID3D11ShaderResourceView*>();
    }

    if (SRVStructuredBuffers.Contains(InSBName) == true && SRVStructuredBuffers[InSBName].Key != nullptr)
    {
        SRVStructuredBuffers[InSBName].Key->Release();
    }
    SRVStructuredBuffers[InSBName].Key = InBuffer;
}

void FRenderResourceManager::AddOrSetSRVStructuredBufferSRV(const FName InSBName, ID3D11ShaderResourceView* InShaderResourceView)
{
    if (SRVStructuredBuffers.Contains(InSBName) == false)
    {
        SRVStructuredBuffers[InSBName] = TPair<ID3D11Buffer*, ID3D11ShaderResourceView*>();
    }

    if (SRVStructuredBuffers.Contains(InSBName) == true && SRVStructuredBuffers[InSBName].Value != nullptr)
    {
        SRVStructuredBuffers[InSBName].Value->Release();
    }
    SRVStructuredBuffers[InSBName].Value = InShaderResourceView;
}

void FRenderResourceManager::AddOrSetUAVStructuredBuffer(const FName InSBName, ID3D11Buffer* InBuffer)
{
    if (UAVStructuredBuffers.Contains(InSBName) == false)
    {
        UAVStructuredBuffers[InSBName] = TPair<ID3D11Buffer*, ID3D11UnorderedAccessView*>();
    }

    if (UAVStructuredBuffers.Contains(InSBName) == true && UAVStructuredBuffers[InSBName].Key != nullptr)
    {
        UAVStructuredBuffers[InSBName].Key->Release();
    }
    UAVStructuredBuffers[InSBName].Key = InBuffer;
}

void FRenderResourceManager::AddOrSetUAVStructuredBufferUAV(const FName InSBName, ID3D11UnorderedAccessView* InUnorderedAccessView)
{
    if (UAVStructuredBuffers.Contains(InSBName) == false)
    {
        UAVStructuredBuffers[InSBName] = TPair<ID3D11Buffer*, ID3D11UnorderedAccessView*>();
    }

    if (UAVStructuredBuffers.Contains(InSBName) == true && UAVStructuredBuffers[InSBName].Value != nullptr)
    {
        UAVStructuredBuffers[InSBName].Value->Release();
    }
    UAVStructuredBuffers[InSBName].Value = InUnorderedAccessView;
}

ID3D11VertexShader* FRenderResourceManager::GetVertexShader(const FName InVSName)
{
    if (VertexShaders.Contains(InVSName))
    {
        return VertexShaders[InVSName]->GetVertexShader();
    }
    return nullptr;
}

ID3DBlob* FRenderResourceManager::GetVertexShaderBlob(const FName InVSName)
{
    if (VertexShaders.Contains(InVSName))
    {
        return VertexShaders[InVSName]->GetShaderBlob();
    }
    return nullptr;
}

ID3D11PixelShader* FRenderResourceManager::GetPixelShader(const FName InPSName)
{
    if (PixelShaders.Contains(InPSName))
    {
        return PixelShaders[InPSName]->GetPixelShader();
    }
    return nullptr;
}

ID3DBlob* FRenderResourceManager::GetPixelShaderBlob(const FName InPSName)
{
    if (PixelShaders.Contains(InPSName))
    {
        return PixelShaders[InPSName]->GetShaderBlob();
    }
    return nullptr;
}

ID3D11InputLayout* FRenderResourceManager::GetInputLayout(const FName InInputLayoutName) const
{
    if (InputLayouts.Contains(InInputLayoutName))
    {
        return InputLayouts[InInputLayoutName];
    }
    return nullptr;
}


ID3D11ComputeShader* FRenderResourceManager::GetComputeShader(const FName InCSName)
{
    if (ComputeShaders.Contains(InCSName))
    {
        return ComputeShaders[InCSName];
    }
    return nullptr;
}

ID3D11Buffer* FRenderResourceManager::GetVertexBuffer(const FName InVBName)
{
    if (VertexBuffers.Contains(InVBName))
    {
        return VertexBuffers[InVBName];
    }
    return nullptr;
}

ID3D11Buffer* FRenderResourceManager::GetIndexBuffer(const FName InIBName)
{
    if (IndexBuffers.Contains(InIBName))
    {
        return IndexBuffers[InIBName];
    }
    return nullptr;
}


ID3D11Buffer* FRenderResourceManager::GetConstantBuffer(const FName InCBName)
{
    if (ConstantBuffers.Contains(InCBName))
    {
        return ConstantBuffers[InCBName];
    }
    
    return nullptr;
}

ID3D11Buffer* FRenderResourceManager::GetSRVStructuredBuffer(const FName InName)
{
    if (SRVStructuredBuffers.Contains(InName))
    {
        return SRVStructuredBuffers[InName].Key;
    }

    return nullptr;
}

ID3D11Buffer* FRenderResourceManager::GetUAVStructuredBuffer(const FName InName)
{
    if (UAVStructuredBuffers.Contains(InName))
    {
        return UAVStructuredBuffers[InName].Key;
    }

    return nullptr;
}

ID3D11ShaderResourceView* FRenderResourceManager::GetStructuredBufferSRV(const FName InName)
{
    if (SRVStructuredBuffers.Contains(InName))
    {
        return SRVStructuredBuffers[InName].Value;
    }

    return nullptr;
}

ID3D11UnorderedAccessView* FRenderResourceManager::GetStructuredBufferUAV(const FName InName)
{
    if (UAVStructuredBuffers.Contains(InName))
    {
        return UAVStructuredBuffers[InName].Value;
    }

    return nullptr;
}

void FRenderResourceManager::HotReloadShaders()
{
    for (const auto VertexShader : VertexShaders)
    {
        if (VertexShader.Value->IsOutDated())
        {
            VertexShader.Value->UpdateShader();
        }
    }

    for (const auto PixelShader : PixelShaders)
    {
        if (PixelShader.Value->IsOutDated())
        {
            PixelShader.Value->UpdateShader();
        }
    }
}
