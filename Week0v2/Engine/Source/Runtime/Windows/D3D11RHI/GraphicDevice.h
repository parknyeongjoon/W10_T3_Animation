#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

#define _TCHAR_DEFINED
#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr; }
#include <d3d11.h>
#include "Container/Array.h"
#include "Container/String.h"

struct FVector4;
enum class EShaderStage;

struct FConstantBufferInfo
{
    FString Name;
    uint32 ByteWidth;
    uint32 BindSlot;
};

class FGraphicsDevice
{
public:
    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* DeviceContext = nullptr;
    IDXGISwapChain* SwapChain = nullptr;
    ID3D11Texture2D* FrameBuffer = nullptr;
    ID3D11Texture2D* UUIDFrameBuffer = nullptr;
    ID3D11RenderTargetView* RTVs[2];
    ID3D11RenderTargetView* FrameBufferRTV = nullptr;
    ID3D11RenderTargetView* UUIDFrameBufferRTV = nullptr;
    ID3D11RasterizerState* RasterizerStateSOLID = nullptr;
    ID3D11RasterizerState* RasterizerStateWIREFRAME = nullptr;
    DXGI_SWAP_CHAIN_DESC SwapchainDesc;
    
    UINT screenWidth = 0;
    UINT screenHeight = 0;
    // Depth-Stencil 관련 변수
    ID3D11Texture2D* DepthStencilBuffer = nullptr;  // 깊이/스텐실 텍스처
    ID3D11DepthStencilView* DepthStencilView = nullptr;  // 깊이/스텐실 뷰
    ID3D11Texture2D* DepthCopyTexture;
    ID3D11ShaderResourceView* DepthCopySRV;
    ID3D11ShaderResourceView* SceneColorSRV = nullptr;
    
    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear) 할 때 사용할 색상(RGBA)

    void Initialize(HWND hWindow);
    void CreateDeviceAndSwapChain(HWND hWindow);
    void CreateDepthStencilBuffer(HWND hWindow);
    bool CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** ppDepthStencilState) const;
    bool CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState) const;
    bool CreateBlendState(const D3D11_BLEND_DESC* pBlendState, ID3D11BlendState** ppBlendState) const;
    void CreateDepthCopyTexture();
    void ReleaseDeviceAndSwapChain();
    void CreateFrameBuffer();
    void ReleaseFrameBuffer();
    void ReleaseDepthStencilResources();
    void Release();
    void SwapBuffer();
    void Prepare();
    void Prepare(D3D11_VIEWPORT* viewport);
    void OnResize(HWND hWindow);
    
    void BindSampler(EShaderStage stage, uint32 StartSlot, uint32 NumSamplers, ID3D11SamplerState* const* ppSamplers) const;
    void BindSamplers(uint32 StartSlot, uint32 NumSamplers, ID3D11SamplerState* const* ppSamplers) const;
    
    // void ChangeDepthStencilState(ID3D11DepthStencilState* newDetptStencil) const;
    ID3D11ShaderResourceView* GetCopiedShaderResourceView() const;

    // Members for Fog
    ID3D11Texture2D* SceneColorTexture = nullptr;
    ID3D11RenderTargetView* SceneColorRTV = nullptr;
    ID3D11Texture2D* RenderTargetTexture = nullptr;
    void CreateSceneColorResources();

    uint32 GetPixelUUID(POINT pt);
    uint32 DecodeUUIDColor(FVector4 UUIDColor);
private:
    //ID3D11RasterizerState* CurrentRasterizer = nullptr;
public:
    static bool CompileVertexShader(const FString& InFileName, const D3D_SHADER_MACRO* pDefines, ID3DBlob** ppCode);
    static bool CompilePixelShader(const FString& InFileName, const D3D_SHADER_MACRO* pDefines, ID3DBlob** ppCode);
    bool CreateVertexShader(const FString& InFileName, const D3D_SHADER_MACRO* pDefines, ID3DBlob** ppCode, ID3D11VertexShader** ppVertexShader) const;
    bool CreatePixelShader(const FString& InFileName, const D3D_SHADER_MACRO* pDefines, ID3DBlob** ppCode, ID3D11PixelShader** ppPixelShader) const;
    static TArray<FConstantBufferInfo> ExtractConstantBufferNames(ID3DBlob* shaderBlob);

};

