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
    VB = renderResourceManager->CreateDynamicVertexBuffer<FSkeletalVertex>(SkeletalMeshRenderData->Vertices);
    renderResourceManager->AddOrSetVertexBuffer(SkeletalMeshRenderData->Name, VB);
    GEngineLoop.Renderer.MappingVBTopology(SkeletalMeshRenderData->Name, SkeletalMeshRenderData->Name, sizeof(FSkeletalVertex), verticeNum);
    const uint32 indexNum = SkeletalMeshRenderData->Indices.Num();
    if (indexNum > 0)
    {
        IB = renderResourceManager->CreateIndexBuffer(SkeletalMeshRenderData->Indices);
        renderResourceManager->AddOrSetIndexBuffer(SkeletalMeshRenderData->Name, IB);
    }
    GEngineLoop.Renderer.MappingIB(SkeletalMeshRenderData->Name, SkeletalMeshRenderData->Name, indexNum);

    MaterialSlots.Empty();
    for (int materialIndex = 0; materialIndex < SkeletalMeshRenderData->Materials.Num(); materialIndex++) {
        FMaterialSlot* newMaterialSlot = new FMaterialSlot();
        UMaterial* newMaterial = FManagerOBJ::CreateMaterial(SkeletalMeshRenderData->Materials[materialIndex]->GetMaterialInfo());

        newMaterialSlot->Material = newMaterial;
        newMaterialSlot->MaterialSlotName = SkeletalMeshRenderData->Materials[materialIndex]->GetMaterialInfo().MTLName;

        MaterialSlots.Add(newMaterialSlot);
    }
}

void USkeletalMesh::UpdateBoneHierarchy() const
{
    // 먼저 루트 뼈들의 글로벌 트랜스폼을 설정
    for (int32 RootIndex : SkeletalMeshRenderData->RootBoneIndices)
    {
        // 루트 뼈는 로컬 트랜스폼이 곧 글로벌 트랜스폼이 됨
        SkeletalMeshRenderData->Bones[RootIndex].GlobalTransform
        = SkeletalMeshRenderData->Bones[RootIndex].LocalTransform;

        SkeletalMeshRenderData->Bones[RootIndex].SkinningMatrix
        = SkeletalMeshRenderData->Bones[RootIndex].InverseBindPoseMatrix
        * SkeletalMeshRenderData->Bones[RootIndex].GlobalTransform;
        
        // 재귀적으로 자식 뼈들의 글로벌 트랜스폼을 업데이트
        UpdateChildBones(RootIndex);
    }
}

void USkeletalMesh::UpdateChildBones(int ParentIndex) const
{
    // BoneTree 구조를 사용하여 현재 부모 뼈의 모든 자식을 찾음
    const FBoneNode& ParentNode = SkeletalMeshRenderData->BoneTree[ParentIndex];
    
    // 모든 자식 뼈를 순회
    for (int32 ChildIndex : ParentNode.ChildIndices)
    {
        // 자식의 글로벌 트랜스폼은 부모의 글로벌 트랜스폼과 자식의 로컬 트랜스폼을 결합한 것
        SkeletalMeshRenderData->Bones[ChildIndex].GlobalTransform
        = SkeletalMeshRenderData->Bones[ChildIndex].LocalTransform
        * SkeletalMeshRenderData->Bones[ParentIndex].GlobalTransform;

        SkeletalMeshRenderData->Bones[ChildIndex].SkinningMatrix
        = SkeletalMeshRenderData->Bones[ChildIndex].InverseBindPoseMatrix
        * SkeletalMeshRenderData->Bones[ChildIndex].GlobalTransform;
        
        // 재귀적으로 이 자식의 자식들도 업데이트
        UpdateChildBones(ChildIndex);
    }
}
