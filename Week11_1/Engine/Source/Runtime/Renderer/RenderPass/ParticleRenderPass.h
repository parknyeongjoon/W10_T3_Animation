#pragma once
#include "Define.h"
#include "FBaseRenderPass.h"
#include "Container/Array.h"
#include "UserInterface/Console.h"
#include "Particles/ParticleHelper.h"
#include "UnrealEd/EditorViewportClient.h"

class UParticleSystemComponent;
class USpotLightComponent;
class ULightComponentBase;
class UStaticMeshComponent;
struct FParticleSpriteVertex;

class FParticleRenderPass : public FBaseRenderPass
{
public:
    explicit FParticleRenderPass(const FName& InShaderName);
    void CreateQuadTextureVertexBuffer();

    virtual ~FParticleRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    //void UpdateComputeConstants(std::shared_ptr<FViewportClient> InViewportClient);
    void CreateDummyTexture();
    void ClearRenderObjects() override;

    EDynamicEmitterType RenderPassEmitterType;

    // void UpdateComputeResource();
private:
    static void UpdateMatrixConstants(UParticleSystemComponent* InParticleSystemComponent, const FMatrix& InView, const FMatrix& InProjection, const FMatrix&
                                      InInvView);
    void UpdateParticleConstants(float InAlpha);
    void UpdateTextureSizeConstants(int InCountX, int InCountY);
    static void UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo);
    void UpdateFlagConstant();
    void UpdateLightConstants();
    void UpdateCameraConstant(const std::shared_ptr<FViewportClient>& InViewportClient);
    void ExtractDescSort(TArray<FParticleSpriteVertex>& OutInstanceVertices, TArray<std::pair<float, FParticleSpriteVertex>>&
                         ParticleVerticesWithDistance);

private:
    bool IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const;
    bool IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const;

    template<typename T>
    static void UpdateInstanceBuffer(ID3D11DeviceContext* DeviceContext, ID3D11Buffer* InBuffer, ID3D11InputLayout* InInputLayout, const TArray<T>& instances, UINT VertexStride)
    {
        // if (instances.Num() == 0 || VertexStride == 0) //디버깅용도로 일단 터뜨리다가 나중에 방어코드 추가
        // {
        //     return;
        // }
        
        uint32 Size = sizeof(T) * instances.Num();

        const uint32 MaxSize = 4096 * 16;
        if (Size > MaxSize)
        {
            UE_LOG(LogLevel::Warning, TEXT("Instance buffer size is too large: %d"), Size);
            Size = (MaxSize / sizeof(T)) * sizeof(T);
        }

        DeviceContext->IASetInputLayout(InInputLayout);
        
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        HRESULT hr = DeviceContext->Map(InBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr)) return;

        memcpy(mapped.pData, instances.GetData(), Size);

        DeviceContext->Unmap(InBuffer, 0);

        UINT InstanceOffset = 0;
        
        DeviceContext->IASetVertexBuffers(1, 1, &InBuffer, &VertexStride, &InstanceOffset);
    }
    
private:        
    TArray<ULightComponentBase*> LightComponents;
    TArray<UParticleSystemComponent*> ParticleSystemComponents;

    ID3D11Buffer* SpriteParticleInstanceBuffer = nullptr;
    ID3D11Buffer* MeshParticleInstanceBuffer = nullptr;
    
    ID3D11ShaderResourceView* DummyWhiteTextureSRV = nullptr;
    ID3D11SamplerState* ShadowMapSampler = nullptr;
    ID3D11Buffer* LightConstantBuffer = nullptr;

};
