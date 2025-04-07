#include "ConstantBufferUpdater.h"
#include <Engine/Texture.h>
#include "Editor/UnrealEd/EditorViewportClient.h"
#include "Engine/Source/Runtime/Engine/Classes/Components/HeightFogComponent.h"
#include "UObject/UObjectIterator.h"
#include "Classes/Components/DirectionalLightComponent.h"
#include "Classes/Components/PointLightComponent.h"
#include "UObject/Casts.h"

void FConstantBufferUpdater::Initialize(ID3D11DeviceContext* InDeviceContext)
{
    DeviceContext = InDeviceContext;
}

void FConstantBufferUpdater::UpdateConstant(ID3D11Buffer* ConstantBuffer, const FMatrix& Model, const FMatrix& ViewProj, const FMatrix& NormalMatrix, FVector4 UUIDColor, bool IsSelected) const
{
    if (ConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

        DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR); // update constant buffer every frame
        {
            FConstants* constants = static_cast<FConstants*>(ConstantBufferMSR.pData);
            constants->Model = Model;
            constants->ViewProj = ViewProj;
            constants->ModelMatrixInverseTranspose = NormalMatrix;
            constants->UUIDColor = UUIDColor;
            constants->IsSelected = IsSelected;
        }
        DeviceContext->Unmap(ConstantBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateMaterialConstant(ID3D11Buffer* MaterialConstantBuffer, const FObjMaterialInfo& MaterialInfo) const
{
    if (MaterialConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

        DeviceContext->Map(MaterialConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR); // update constant buffer every frame
        {
            FMaterialConstants* constants = static_cast<FMaterialConstants*>(ConstantBufferMSR.pData);
            constants->DiffuseColor = MaterialInfo.Diffuse;
            constants->TransparencyScalar = MaterialInfo.TransparencyScalar;
            constants->AmbientColor = MaterialInfo.Ambient;
            constants->DensityScalar = MaterialInfo.DensityScalar;
            constants->SpecularColor = MaterialInfo.Specular;
            constants->SpecularScalar = MaterialInfo.SpecularScalar;
            constants->EmmisiveColor = MaterialInfo.Emissive;
        }
        DeviceContext->Unmap(MaterialConstantBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateLightConstant(ID3D11Buffer* LightingBuffer, const TArray<ULightComponentBase*>& LightObjs) const
{
    if (!LightingBuffer) return;

    FLightingConstant lightingData;
    lightingData.NumDirectionalLights = 0;
    lightingData.NumPointLights = 0;
    // 주변광은 우선 하드코딩한다
    lightingData.AmbientColor = FVector(0.1f, 0.1f, 0.1f);
    lightingData.AmbientIntensity = 1.0f;

    for (const auto& Light : LightObjs)
    {
        if (Light->IsA<UDirectionalLightComponent>())
        {
            UDirectionalLightComponent* DirLight = Cast<UDirectionalLightComponent>(Light);
            if (DirLight && lightingData.NumDirectionalLights < MAX_DIRECTIONAL_LIGHTS)
            {
                lightingData.DirLights[lightingData.NumDirectionalLights].Direction = DirLight->GetDirection();
                lightingData.DirLights[lightingData.NumDirectionalLights].Intensity = DirLight->GetIntensity();
                lightingData.DirLights[lightingData.NumDirectionalLights].Color = DirLight->GetColor().xyz();
                lightingData.NumDirectionalLights++;
            }
        }
        else if (Light->IsA<UPointLightComponent>())
        {
            UPointLightComponent* PointLight = Cast<UPointLightComponent>(Light);
            if (PointLight && lightingData.NumPointLights < MAX_POINT_LIGHTS)
            {
                lightingData.PointLights[lightingData.NumPointLights].Position = PointLight->GetWorldLocation();
                lightingData.PointLights[lightingData.NumPointLights].Radius = PointLight->GetRadius();
                lightingData.PointLights[lightingData.NumPointLights].Color = PointLight->GetColor().xyz();
                lightingData.PointLights[lightingData.NumPointLights].Intensity = PointLight->GetIntensity();
                lightingData.PointLights[lightingData.NumPointLights].AttenuationFalloff = PointLight->GetAttenuationFalloff();
                lightingData.NumPointLights++;
            }
        }
    }
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = DeviceContext->Map(LightingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &lightingData, sizeof(FLightingConstant));
        DeviceContext->Unmap(LightingBuffer, 0);
    }
}


void FConstantBufferUpdater::UpdateLitUnlitConstant(ID3D11Buffer* FlagBuffer, int isLit) const
{
    if (FlagBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
        DeviceContext->Map(FlagBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
        auto constants = static_cast<FLitUnlitConstants*>(constantbufferMSR.pData);
        {
            constants->isLit = isLit;
        }
        DeviceContext->Unmap(FlagBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateSubMeshConstant(ID3D11Buffer* SubMeshConstantBuffer, bool isSelected) const
{
    if (SubMeshConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
        DeviceContext->Map(SubMeshConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
        FSubMeshConstants* constants = (FSubMeshConstants*)constantbufferMSR.pData;
        {
            constants->isSelectedSubMesh = isSelected;
        }
        DeviceContext->Unmap(SubMeshConstantBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateTextureConstant(ID3D11Buffer* TextureConstantBufer, float UOffset, float VOffset)
{
    if (TextureConstantBufer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
        DeviceContext->Map(TextureConstantBufer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
        FTextureConstants* constants = (FTextureConstants*)constantbufferMSR.pData;
        {
            constants->UOffset = UOffset;
            constants->VOffset = VOffset;
        }
        DeviceContext->Unmap(TextureConstantBufer, 0);
    }
}

void FConstantBufferUpdater::UpdateSubUVConstant(ID3D11Buffer* SubUVConstantBuffer, float _indexU, float _indexV) const
{
    if (SubUVConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

        DeviceContext->Map(SubUVConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
        auto constants = static_cast<FSubUVConstant*>(constantbufferMSR.pData);
        {
            constants->indexU = _indexU;
            constants->indexV = _indexV;
        }
        DeviceContext->Unmap(SubUVConstantBuffer, 0);
    }
}


void FConstantBufferUpdater::UpdateCameraConstant(ID3D11Buffer* CameraConstantBuffer, FEditorViewportClient* ViewportClient)
{
    if (CameraConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
        DeviceContext->Map(CameraConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
        auto constants = static_cast<FCameraConstants*>(constantbufferMSR.pData);
        {
            constants->ViewMatrix = FMatrix::Transpose(ViewportClient->GetViewMatrix());
            constants->ProjMatrix = FMatrix::Transpose(ViewportClient->GetProjectionMatrix());
            constants->InvViewMatrix = FMatrix::Transpose(FMatrix::Inverse(ViewportClient->GetViewMatrix()));
            constants->InvProjMatrix = FMatrix::Transpose(FMatrix::Inverse(ViewportClient->GetProjectionMatrix()));
            constants->CameraPos = ViewportClient->ViewTransformPerspective.GetLocation();
            constants->NearPlane = ViewportClient->nearPlane;
            constants->CameraForward = ViewportClient->ViewTransformPerspective.GetForwardVector();
            constants->FarPlane = ViewportClient->farPlane;
        }
        DeviceContext->Unmap(CameraConstantBuffer, 0);
    }
}