#include "SkeletalMesh.h"

#include "Renderer/RenderResourceManager.h"
#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"
#include "TestFBXLoader.h"
#include "Components/Material/Material.h"
#include "Engine/FLoaderOBJ.h"

uint32 USkeletalMesh::GetMaterialIndex(FName MaterialSlotName) const
{
    for (uint32 materialIndex = 0; materialIndex < MaterialSlots.Num(); materialIndex++) {
        if (MaterialSlots[materialIndex]->MaterialSlotName == MaterialSlotName)
            return materialIndex;
    }

    return -1;
}

void USkeletalMesh::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    for (const FMaterialSlot* Material : MaterialSlots)
    {
        Out.Emplace(Material->Material);
    }
}

void USkeletalMesh::SetData(FSkeletalMeshRenderData* renderData)
{
    SkeletalMeshRenderData = renderData;

    ID3D11Buffer* VB = nullptr; 
    ID3D11Buffer* IB = nullptr;

    const uint32 verticeNum = SkeletalMeshRenderData->Vertices.Num();
    if (verticeNum <= 0) return;

    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    VB = renderResourceManager->CreateImmutableVertexBuffer<FSkeletalVertex>(SkeletalMeshRenderData->Vertices);
    renderResourceManager->AddOrSetVertexBuffer(SkeletalMeshRenderData->Name, VB);
    GEngineLoop.Renderer.MappingVBTopology(SkeletalMeshRenderData->Name, SkeletalMeshRenderData->Name, sizeof(FVertexSimple), verticeNum);
    
    const uint32 indexNum = SkeletalMeshRenderData->Indices.Num();
    if (indexNum > 0)
    {
        IB = renderResourceManager->CreateIndexBuffer(SkeletalMeshRenderData->Indices);
        renderResourceManager->AddOrSetIndexBuffer(SkeletalMeshRenderData->Name, IB);
    }
    GEngineLoop.Renderer.MappingIB(SkeletalMeshRenderData->Name, SkeletalMeshRenderData->Name, indexNum);
    
    for (int materialIndex = 0; materialIndex < SkeletalMeshRenderData->Materials.Num(); materialIndex++) {
        FMaterialSlot* newMaterialSlot = new FMaterialSlot();
        UMaterial* newMaterial = FManagerOBJ::CreateMaterial(SkeletalMeshRenderData->Materials[materialIndex]->GetMaterialInfo());

        newMaterialSlot->Material = newMaterial;
        newMaterialSlot->MaterialSlotName = SkeletalMeshRenderData->Materials[materialIndex]->GetMaterialInfo().MTLName;

        MaterialSlots.Add(newMaterialSlot);
    }
}
