#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define _TCHAR_DEFINED
#include "EngineBaseTypes.h"
#include "Define.h"
#include "RenderResourceManager.h"

class FEditorViewportClient;
class UWorld;

class FRenderer 
{
private:
    void CreateStaticMeshShader();
    void CreateTextureShader();
    void CreateLineShader();
    void CreateFogShader();
    void CreateDebugDepthShader();
public:
    FGraphicsDevice* Graphics;
    // ID3D11VertexShader* VertexShader = nullptr;
    // ID3D11PixelShader* PixelShader = nullptr;
    // ID3D11InputLayout* InputLayout = nullptr;
    // ID3D11Buffer* ConstantBuffer = nullptr;
    // ID3D11Buffer* LightingBuffer = nullptr;
    // ID3D11Buffer* FlagBuffer = nullptr;
    // ID3D11Buffer* MaterialConstantBuffer = nullptr;
    // ID3D11Buffer* SubMeshConstantBuffer = nullptr;
    // ID3D11Buffer* TextureConstantBufer = nullptr;
    // ID3D11Buffer* CameraConstantBuffer = nullptr;
    // ID3D11Buffer* ViewportConstantBuffer = nullptr;
    // ID3D11Buffer* DepthToWorldBuffer = nullptr;

    //ID3D11BlendState* NormalBlendState = nullptr;

    //FLightingConstant lightingData;

    //uint32 Stride;
    //uint32 Stride2;
public:
    ID3D11SamplerState* GetSamplerState(const ESamplerType InType) const { return RenderResourceManager.GetSamplerState(InType); }
    ID3D11RasterizerState* GetRasterizerState(const ERasterizerState InState) const { return RenderResourceManager.GetRasterizerState(InState); }
    ID3D11BlendState* GetBlendState(const EBlendState InState) const { return RenderResourceManager.GetBlendState(InState); }
    ID3D11DepthStencilState* GetDepthStencilState(const EDepthStencilState InState) const { return RenderResourceManager.GetDepthStencilState(InState); }

    ID3D11RasterizerState* GetCurrentRasterizerState() const {  return RenderResourceManager.GetRasterizerState(CurrentRasterizerState); }
    void SetCurrentRasterizerState(const ERasterizerState InState) { CurrentRasterizerState = InState; }
public:
    void Initialize(FGraphicsDevice* graphics);
   
    void PrepareShader() const;
    
    //Render
    void RenderPrimitive(OBJ::FStaticMeshRenderData* renderData, TArray<FStaticMaterial*> materials, TArray<UMaterial*> overrideMaterial, int selectedSubMeshIndex) const;
   
    //Release
    void Release();
    void ReleaseShader();

    void CreateShader();
    
    void ChangeViewMode(EViewModeIndex evi) const;

    // update
    void UpdateMaterial(const FObjMaterialInfo& MaterialInfo) const;
public://텍스쳐용 기능 추가

    uint32 TextureStride;
    struct FSubUVConstant
    {
        float indexU;
        float indexV;
    };
    //ID3D11Buffer* SubUVConstantBuffer = nullptr;

public:
    void PrepareTextureShader() const;

    void RenderTexturePrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices,
        ID3D11Buffer* pIndexBuffer, UINT numIndices,
        ID3D11ShaderResourceView* _TextureSRV,
        ID3D11SamplerState* _SamplerState) const;
    void RenderTextPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices,
        ID3D11ShaderResourceView* _TextureSRV,
        ID3D11SamplerState* _SamplerState) const;

    void PrepareSubUVConstant() const;

public: // line shader
    void PrepareLineShader() const;
    void UpdateLinePrimitveCountBuffer(int numBoundingBoxes, int numCones) const;
    
    //Render Pass Demo
    void PrepareRender();
    void ClearRenderArr();
    void Render(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void Render(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport, std::shared_ptr<FEditorViewportClient> CurrentViewport);
    void RenderStaticMeshes(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderGizmos(const UWorld* World, const std::shared_ptr<FEditorViewportClient>& ActiveViewport);
    void RenderLight(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderBillboards(UWorld* World,std::shared_ptr<FEditorViewportClient> ActiveViewport);

    // post process
    void RenderPostProcess(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport, std::shared_ptr<FEditorViewportClient> CurrentViewport);
    void RenderDebugDepth(std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void RenderHeightFog(std::shared_ptr<FEditorViewportClient> ActiveViewport, std::shared_ptr<FEditorViewportClient> CurrentViewport);

public:
    void MappingVSPSInputLayout(FName InShaderProgramName, FName VSName, FName PSName, ID3D11InputLayout* InputLayout);
    void MappingVSPSCBSlot(FName InShaderName, const TMap<FShaderConstantKey, uint32>& MappedConstants);
private: 
    TMap<FName, std::shared_ptr<FShaderProgram>> ShaderPrograms;
    TMap<FName, TMap<FShaderConstantKey, uint32>> ShaderConstantNameAndSlots;
    TMap<FName, std::shared_ptr<FVIBuffers>> VIBuffers;
public:
    FRenderResourceManager& GetResourceManager() { return RenderResourceManager; }
private:
    FRenderResourceManager RenderResourceManager;

    ERasterizerState CurrentRasterizerState = ERasterizerState::SolidBack;
};

