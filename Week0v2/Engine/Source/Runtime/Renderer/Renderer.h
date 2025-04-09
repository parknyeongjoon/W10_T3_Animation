#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define _TCHAR_DEFINED
#include <d3d11.h>
#include "EngineBaseTypes.h"
#include "Define.h"
#include "Container/Set.h"
#include "RenderResourceManager.h"
#include "ShaderManager.h"
#include "ConstantBufferUpdater.h"

class ULightComponentBase;
class UWorld;
class FGraphicsDevice;
class UMaterial;
struct FStaticMaterial;
class UObject;
class FEditorViewportClient;
class UBillboardComponent;
class UStaticMeshComponent;
class UGizmoBaseComponent;
class FRenderResourceManager;
class FRenderer 
{

private:
    float litFlag = 0;
public:
    FGraphicsDevice* Graphics;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    ID3D11Buffer* ConstantBuffer = nullptr;
    ID3D11Buffer* LightingBuffer = nullptr;
    ID3D11Buffer* FlagBuffer = nullptr;
    ID3D11Buffer* MaterialConstantBuffer = nullptr;
    ID3D11Buffer* SubMeshConstantBuffer = nullptr;
    ID3D11Buffer* TextureConstantBufer = nullptr;
    ID3D11Buffer* CameraConstantBuffer = nullptr;
    ID3D11Buffer* ViewportConstantBuffer = nullptr;
    ID3D11Buffer* DepthToWorldBuffer = nullptr;

    ID3D11BlendState* NormalBlendState = nullptr;

    FLightingConstant lightingData;

    uint32 Stride;
    uint32 Stride2;

public:
    void Initialize(FGraphicsDevice* graphics);
   
    void PrepareShader() const;
    
    //Render
    void RenderPrimitive(OBJ::FStaticMeshRenderData* renderData, TArray<FStaticMaterial*> materials, TArray<UMaterial*> overrideMaterial, int selectedSubMeshIndex) const;
   
    //Release
    void Release();
    void ReleaseShader();
    void ReleaseConstantBuffer();
    void CreateBlendState();
    void ReleaseBlendState();

    void CreateShader();
    
    void ChangeViewMode(EViewModeIndex evi) const;
    
    // CreateBuffer
    void CreateConstantBuffer();

    // update
    void UpdateMaterial(const FObjMaterialInfo& MaterialInfo) const;


public://텍스쳐용 기능 추가
    ID3D11VertexShader* VertexTextureShader = nullptr;
    ID3D11PixelShader* PixelTextureShader = nullptr;
    ID3D11InputLayout* TextureInputLayout = nullptr;

	// 임시
	ID3D11VertexShader* DebugDepthVertexShader = nullptr;
	ID3D11PixelShader* DebugDepthPixelShader = nullptr;
    ID3D11VertexShader* HeightFogVertexShader = nullptr;
    ID3D11PixelShader* HeightFogPixelShader = nullptr;
	// sampler
	ID3D11SamplerState* DebugDepthSRVSampler = nullptr;

    // Shaders for fog
    // ID3D11VertexShader* HeightFogVertexShader;
    // ID3D11PixelShader* HeightFogPixelShader;
    ID3D11Buffer* FogConstantBuffer;

    uint32 TextureStride;
    struct FSubUVConstant
    {
        float indexU;
        float indexV;
    };
    ID3D11Buffer* SubUVConstantBuffer = nullptr;

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
    void RenderBatch(const FGridParameters& gridParam, ID3D11Buffer* pVertexBuffer, int boundingBoxCount, int coneCount, int coneSegmentCount, int obbCount) const;
    void UpdateGridConstantBuffer(const FGridParameters& gridParams) const;
    void UpdateLinePrimitveCountBuffer(int numBoundingBoxes, int numCones) const;

    ID3D11ShaderResourceView* CreateBoundingBoxSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes);
    ID3D11ShaderResourceView* CreateOBBSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes);
    ID3D11ShaderResourceView* CreateConeSRV(ID3D11Buffer* pConeBuffer, UINT numCones);

    void UpdateBoundingBoxBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FBoundingBox>& BoundingBoxes, int numBoundingBoxes) const;
    void UpdateOBBBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FOBB>& BoundingBoxes, int numBoundingBoxes) const;
    void UpdateConesBuffer(ID3D11Buffer* pConeBuffer, const TArray<FCone>& Cones, int numCones) const;

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
private:
    TArray<UStaticMeshComponent*> StaticMeshObjs;
    TArray<UGizmoBaseComponent*> GizmoObjs;
    TArray<UBillboardComponent*> BillboardObjs;
    TArray<ULightComponentBase*> LightObjs;

public:
    ID3D11VertexShader* VertexLineShader = nullptr;
    ID3D11PixelShader* PixelLineShader = nullptr;
    ID3D11Buffer* GridConstantBuffer = nullptr;
    ID3D11Buffer* LinePrimitiveBuffer = nullptr;
    ID3D11ShaderResourceView* pBBSRV = nullptr;
    ID3D11ShaderResourceView* pConeSRV = nullptr;
    ID3D11ShaderResourceView* pOBBSRV = nullptr;

public:
    FRenderResourceManager& GetResourceManager() { return RenderResourceManager; }
    FShaderManager& GetShaderManager() { return ShaderManager; }
    FConstantBufferUpdater& GetConstantBufferUpdater() { return ConstantBufferUpdater; }

private:
    FRenderResourceManager RenderResourceManager;
    FShaderManager ShaderManager;
    FConstantBufferUpdater ConstantBufferUpdater;
};

