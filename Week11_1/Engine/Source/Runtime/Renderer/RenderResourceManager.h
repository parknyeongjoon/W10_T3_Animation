#pragma once

#include "Define.h"
#include "Container/Map.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UserInterface/Console.h"

class FPixelShader;
class FVertexShader;
class FVBIBTopologyMapping;
class FShaderProgram;

class FRenderResourceManager
{
public:
    FRenderResourceManager(FGraphicsDevice* InGraphicDevice);
    void Initialize();
    void LoadStates();

    void ReleaseResources();

    template<typename T>
    ID3D11Buffer* CreateImmutableVertexBuffer(const TArray<T>& vertices) const;
    template<typename T>
    ID3D11Buffer* CreateImmutableVertexBuffer(T* vertices, uint32 arraySize) const;
    
    template <typename T>
    ID3D11Buffer* CreateStructuredBuffer(uint32 numElements) const;
    template <class T>
    ID3D11Buffer* CreateUAVStructuredBuffer(uint32 numElements) const;

    template<typename T>
    ID3D11Buffer* CreateStaticVertexBuffer(const TArray<T>& vertices) const;
    template<typename T>
    ID3D11Buffer* CreateStaticVertexBuffer(T* vertices, uint32 arraySize) const;
    
    template<typename T>
    ID3D11Buffer* CreateDynamicVertexBuffer(const TArray<T>& vertices) const;
    template<typename T>
    ID3D11Buffer* CreateDynamicVertexBuffer(T* vertices, uint32 arraySize) const;
    
    ID3D11Buffer* CreateIndexBuffer(const uint32* indices, uint32 indicesSize) const;
    ID3D11Buffer* CreateIndexBuffer(const TArray<uint32>& indices) const;
    
    ID3D11Buffer* CreateConstantBuffer(uint32 InSize, const void* InData = nullptr) const;
    
    template<typename T>
    void UpdateConstantBuffer(ID3D11Buffer* InBuffer, const T* InData = nullptr);

    template<typename T>
    void UpdateConstantBuffer(const FName& CBName, const T* InData = nullptr);

    template<typename T>
    void UpdateDynamicVertexBuffer(ID3D11Buffer* InBuffer, T* vertices, const uint32 numVertices) const;
    
    template <typename T>
    void UpdateStructuredBuffer(ID3D11Buffer* pBuffer, const TArray<T>& Data) const;
    
    ID3D11ShaderResourceView* CreateBufferSRV(ID3D11Buffer* pBuffer, UINT numElements) const;
    ID3D11UnorderedAccessView* CreateBufferUAV(ID3D11Buffer* pBuffer, UINT numElements) const;

    ID3D11SamplerState* GetSamplerState(ESamplerType InType) const { return SamplerStates[static_cast<uint32>(InType)]; }
    ID3D11RasterizerState* GetRasterizerState(ERasterizerState InState) const { return RasterizerStates[static_cast<uint32>(InState)]; }
    ID3D11BlendState* GetBlendState(EBlendState InState) const { return BlendStates[static_cast<uint32>(InState)]; }
    ID3D11DepthStencilState* GetDepthStencilState(EDepthStencilState InState) const { return DepthStencilStates[static_cast<uint32>(InState)]; }

    void AddOrSetComputeShader(FName InCSName, ID3D11ComputeShader* InShader);
    
    void CreateVertexShader(const FString& InShaderName, const FString& InFileName, D3D_SHADER_MACRO* pDefines);
    void UpdateVertexShader(const FString& InShaderName, const FString& InFullPath, D3D_SHADER_MACRO* pDefines);
    
    void CreatePixelShader(const FString& InShaderName, const FString& InFileName, D3D_SHADER_MACRO* pDefines);
    void UpdatePixelShader(const FString& InShaderName, const FString& InFullPath, D3D_SHADER_MACRO* pDefines);
    
    void AddOrSetVertexShader(FName InVSName, const FString& InFullPath, ID3D11VertexShader* InVS, ID3DBlob* InShaderBlob, D3D_SHADER_MACRO*
                              InShaderMacro, std::filesystem::file_time_type InWriteTime);
    void AddOrSetPixelShader(FName InPSName, const FString& InFullPath, ID3D11PixelShader* InPS, ID3DBlob* InShaderBlob, D3D_SHADER_MACRO*
                             InShaderMacro, std::filesystem::file_time_type InWriteTime);
    void AddOrSetInputLayout(FName InInputLayoutName, ID3D11InputLayout* InInputLayout);

    void AddOrSetVertexBuffer(FName InVBName, ID3D11Buffer* InBuffer);
    void AddOrSetIndexBuffer(FName InPBName, ID3D11Buffer* InBuffer);
    void AddOrSetConstantBuffer(FName InCBName, ID3D11Buffer* InBuffer);
    void AddOrSetSRVStructuredBuffer(FName InSBName, ID3D11Buffer* InBuffer);
    void AddOrSetSRVStructuredBufferSRV(FName InSBName, ID3D11ShaderResourceView* InShaderResourceView);
    void AddOrSetUAVStructuredBuffer(FName InSBName, ID3D11Buffer* InBuffer);
    void AddOrSetUAVStructuredBufferUAV(FName InSBName, ID3D11UnorderedAccessView* InUnorderedAccessView);
    
    ID3D11VertexShader* GetVertexShader(const FName InVSName);
    ID3DBlob* GetVertexShaderBlob(const FName InVSName);
    ID3D11PixelShader* GetPixelShader(const FName InPSName);
    ID3D11ComputeShader* GetComputeShader(const FName InCSName);

    ID3DBlob* GetPixelShaderBlob(const FName InPSName);

    ID3D11InputLayout* GetInputLayout(const FName InInputLayoutName) const;

    ID3D11Buffer* GetVertexBuffer(const FName InVBName);
    ID3D11Buffer* GetIndexBuffer(const FName InIBName);
    ID3D11Buffer* GetConstantBuffer(const FName InCBName);

    ID3D11Buffer* GetSRVStructuredBuffer(FName InName);
    ID3D11Buffer* GetUAVStructuredBuffer(FName InName);
    ID3D11ShaderResourceView* GetStructuredBufferSRV(const FName InName);
    ID3D11UnorderedAccessView* GetStructuredBufferUAV(const FName InName);

    void HotReloadShaders();
private:
    FGraphicsDevice* GraphicDevice = nullptr;
    
private:
    TMap<FName, ID3D11ComputeShader*> ComputeShaders;
    TMap<FName, std::shared_ptr<FVertexShader>> VertexShaders;
    TMap<FName, std::shared_ptr<FPixelShader>> PixelShaders;
    TMap<FName, ID3D11InputLayout*> InputLayouts;

    TMap<FName, ID3D11Buffer*> VertexBuffers;
    TMap<FName, ID3D11Buffer*> IndexBuffers;
    TMap<FName, ID3D11Buffer*> ConstantBuffers;

    TMap<FName, TPair<ID3D11Buffer*, ID3D11ShaderResourceView*>> SRVStructuredBuffers;
    TMap<FName, TPair<ID3D11Buffer*, ID3D11UnorderedAccessView*>> UAVStructuredBuffers;
private:
    ID3D11SamplerState* SamplerStates[static_cast<uint32>(ESamplerType::End)] = {};
      
    ID3D11RasterizerState* RasterizerStates[static_cast<uint32>(ERasterizerState::End)] = {};
    
    ID3D11BlendState* BlendStates[static_cast<uint32>(EBlendState::End)] = {};
    
    ID3D11DepthStencilState* DepthStencilStates[static_cast<uint32>(EDepthStencilState::End)] = {};
};

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateImmutableVertexBuffer(const TArray<T>& vertices) const
{
    D3D11_BUFFER_DESC vertexbufferdesc = {};
    vertexbufferdesc.ByteWidth = sizeof(T) * vertices.Num();
    vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // 한 번 생성 후 업데이트하지 않음
    vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexbufferSRD = {};
    vertexbufferSRD.pSysMem = vertices.GetData();
    ID3D11Buffer* vertexBuffer = nullptr;

    const HRESULT hr = GraphicDevice->Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "VertexBuffer Creation failed");
        return nullptr;
    }

    
    return vertexBuffer;
}

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateImmutableVertexBuffer(T* vertices, uint32 arraySize) const
{
    TArray<T> verticeArray;
    verticeArray.AppendArray(vertices, arraySize);

    return CreateImmutableVertexBuffer(verticeArray);
}

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateStructuredBuffer(const uint32 numElements) const
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // CPU가 데이터를 업데이트할 수 있도록 설정
    bufferDesc.ByteWidth = sizeof(T) * numElements;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = sizeof(T);

    ID3D11Buffer* buffer = nullptr;
    const HRESULT hr = GraphicDevice->Device->CreateBuffer(&bufferDesc, nullptr, &buffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "Structured Buffer Creation failed");
        return nullptr;
    }
    return buffer;
}

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateUAVStructuredBuffer(const uint32 numElements) const
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(T) * numElements;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = sizeof(T);

    ID3D11Buffer* buffer = nullptr;
    const HRESULT hr = GraphicDevice->Device->CreateBuffer(&bufferDesc, nullptr, &buffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "Structured Buffer Creation failed");
        return nullptr;
    }
    return buffer;
}

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateStaticVertexBuffer(const TArray<T>& vertices) const
{
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;  // 정적 버퍼: 한 번 생성 후 업데이트하지 않음
    vbDesc.ByteWidth =  sizeof(T) * vertices.Num(); // 정점 데이터 개수에 따라 총 바이트 크기를 계산합니다.
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;           // CPU에서 직접 접근하지 않음

    D3D11_SUBRESOURCE_DATA vbInitData = {};
    vbInitData.pSysMem = vertices.GetData();

    ID3D11Buffer* pVertexBuffer = nullptr;
    const HRESULT hr = GraphicDevice->Device->CreateBuffer(&vbDesc, &vbInitData, &pVertexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "Static Vertex Buffer Creation failed");
        return nullptr;
    }
    return pVertexBuffer;
}

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateStaticVertexBuffer(T* vertices, uint32 arraySize) const
{
    TArray<T> verticeArray;
    verticeArray.AppendArray(vertices, arraySize);

    return CreateStaticVertexBuffer(verticeArray);
}

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateDynamicVertexBuffer(const TArray<T>& vertices) const
{
    D3D11_BUFFER_DESC vertexbufferdesc = {};
    vertexbufferdesc.ByteWidth = sizeof(T) * vertices.Num();
    vertexbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // 업데이트 가능
    vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA vertexbufferSRD = {};
    vertexbufferSRD.pSysMem = vertices.GetData();

    ID3D11Buffer* vertexBuffer = nullptr;

    const HRESULT hr = GraphicDevice->Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Warning, "VertexBuffer Creation failed");
        return nullptr;
    }
    return vertexBuffer;
}

template <typename T>
ID3D11Buffer* FRenderResourceManager::CreateDynamicVertexBuffer(T* vertices, uint32 arraySize) const
{
    TArray<T> verticeArray;
    verticeArray.AppendArray(vertices, arraySize);

    return CreateDynamicVertexBuffer(verticeArray);
}

template <typename T>
void FRenderResourceManager::UpdateConstantBuffer(ID3D11Buffer* InBuffer, const T* InData)
{
    D3D11_MAPPED_SUBRESOURCE sub = {};
    const HRESULT hr = GraphicDevice->DeviceContext->Map(InBuffer, 0,D3D11_MAP_WRITE_DISCARD,0, &sub);
    if (FAILED(hr))
    {
        // 실패 시 메시지 박스 표시
        std::string errorMessage = "Map failed for Constant buffer type: ";
        errorMessage += typeid(T).name(); // T의 이름 추가
        MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
        return;
    }
    memcpy(sub.pData, InData, sizeof(T));
    GraphicDevice->DeviceContext->Unmap(InBuffer, 0);
}

template <typename T>
void FRenderResourceManager::UpdateConstantBuffer(const FName& CBName, const T* InData)
{
    ID3D11Buffer* CB = GetConstantBuffer(CBName);
    if (CB == nullptr)
    {
        UE_LOG(LogLevel::Error, TEXT("UpdateConstantBuffer 호출: 키 %s에 해당하는 buffer가 없습니다."), CBName);
        return;
    }
    
    UpdateConstantBuffer(CB, InData);
}

template <typename T>
void FRenderResourceManager::UpdateDynamicVertexBuffer(ID3D11Buffer* InBuffer, T* vertices, const uint32 numVertices) const
{
    D3D11_MAPPED_SUBRESOURCE sub = {};
    const HRESULT hr = GraphicDevice->DeviceContext->Map(InBuffer, 0,D3D11_MAP_WRITE_DISCARD,0, &sub);
    if (FAILED(hr))
    {
        // 실패 시 메시지 박스 표시
        std::string errorMessage = "Map failed for Dynamic VertexBuffer type: ";
        errorMessage += typeid(T).name(); // T의 이름 추가
        MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
        return;
    }
    memcpy(sub.pData, vertices, sizeof(T) * numVertices);
    GraphicDevice->DeviceContext->Unmap(InBuffer, 0);
}

template <typename T>
void FRenderResourceManager::UpdateStructuredBuffer(ID3D11Buffer* pBuffer, const TArray<T>& Data) const
{
    if (!pBuffer)
        return;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    const HRESULT hr = GraphicDevice->DeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
        // 실패 시 메시지 박스 표시
        std::string errorMessage = "Map failed for StructuredBuffer type: ";
        errorMessage += typeid(T).name(); // T의 이름 추가
        MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
        return;
    }

    auto pData = reinterpret_cast<T*>(mappedResource.pData);
    for (int i = 0; i < Data.Num(); ++i)
    {
        pData[i] = Data[i];
    }

    GraphicDevice->DeviceContext->Unmap(pBuffer, 0);
}