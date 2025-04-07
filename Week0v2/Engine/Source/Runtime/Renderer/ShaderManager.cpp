#include "ShaderManager.h"
#include <d3dcompiler.h>

void FShaderManager::Initialize(ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext)
{
    Device = InDevice;
    DeviceContext = InDeviceContext;
}

void FShaderManager::Release()
{
    Device = nullptr;
}

bool FShaderManager::CreateVertexShader(
    const FWString& vsPath,
    const FString& vsEntry,
    ID3D11VertexShader*& outVS,
    const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc,
    UINT numElements,
    ID3D11InputLayout** outInputLayout,
    UINT* outStride,
    UINT vertexSize)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(vsPath.c_str(), nullptr, nullptr, *vsEntry, "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr))
    {
        // Output error message if compilation fails
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        else
        {
            OutputDebugStringA("Unknown error during shader compilation.");
        }
        return false;
    }
    hr = Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &outVS);
    if (FAILED(hr))
    {
        vsBlob->Release(); return false;
    }

    if (outInputLayout)
    {
        hr = Device->CreateInputLayout(inputLayoutDesc, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), outInputLayout);
        if (FAILED(hr)) { vsBlob->Release(); return false; }
    }

    if (outStride)
        *outStride = vertexSize;

    vsBlob->Release();
    return true;
}

bool FShaderManager::CreatePixelShader(
    const FWString& psPath,
    const FString& psEntry,
    ID3D11PixelShader*& outPS)
{
    ID3DBlob* psBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(psPath.c_str(), nullptr, nullptr, *psEntry, "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr)) return false;

    hr = Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &outPS);
    psBlob->Release();
    return SUCCEEDED(hr);
}

void FShaderManager::ReleaseShader(ID3D11InputLayout* layout, ID3D11VertexShader* vs, ID3D11PixelShader* ps)
{
    if (layout)
    {
        layout->Release();
        layout = nullptr;
    }
    if (vs)
    {
        vs->Release();
        vs = nullptr;
    }
    if (ps)
    {
        ps->Release();
        ps = nullptr;
    }
}

//void FShaderManager::ClearVertexShader(ID3D11VertexShader* VertexShader)
//{
//    if (VertexShader)
//    {
//        DeviceContext->VSSetShader(nullptr, nullptr, 0);
//        VertexShader->Release();
//        VertexShader = nullptr;
//    }
//}
//
//void FShaderManager::ClearPixelShader(ID3D11PixelShader* PixelShader)
//{
//    if (PixelShader)
//    {
//        DeviceContext->PSSetShader(nullptr, nullptr, 0);
//        PixelShader->Release();
//        PixelShader = nullptr;
//    }
//}
//
//void FShaderManager::SetVertexShader(ID3D11VertexShader* VertexShader, const FWString& filename, const FString& funcname, const FString& version)
//{
//    if (VertexShader != nullptr)
//        ClearVertexShader(VertexShader);
//
//    ID3DBlob* vertexshaderCSO;
//
//    D3DCompileFromFile(filename.c_str(), nullptr, nullptr, *funcname, *version, 0, 0, &vertexshaderCSO, nullptr);
//    Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &VertexShader);
//    vertexshaderCSO->Release();
//}
//
//void FShaderManager::SetPixelShader(ID3D11PixelShader* PixelShader, const FWString& filename, const FString& funcname, const FString& version)
//{
//    if (PixelShader != nullptr)
//        ClearPixelShader(PixelShader);
//
//    ID3DBlob* pixelshaderCSO;
//    D3DCompileFromFile(filename.c_str(), nullptr, nullptr, *funcname, *version, 0, 0, &pixelshaderCSO, nullptr);
//    Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &PixelShader);
//
//    pixelshaderCSO->Release();
//}
//
