#pragma once
#include <fbxsdk.h>

#include "Container/Map.h"
#include "FBX/FBXDefine.h"
#include "UObject/NameTypes.h"

class TestFBXLoader
{
public:
    bool InitFBXManager();
    bool InitFBX(const FString& FilePath);
    void ExtractFBXMeshData(const FbxScene* Scene, FSkeletalMeshRenderData* MeshData) const;
    void ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData) const;
    void ExtractVertices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData) const;
    void ExtractNormals(FbxMesh* Mesh, FSkeletalMeshRenderData* RenderData, int BaseVertexIndex) const;
    void ExtractUVs(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex) const;
    void ExtractTangents(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex) const;
    void ExtractSkinningData(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex) const;
    void ProcessSkinning(FbxSkin* skin, FSkeletalMeshRenderData* mesh_data, int base_vertex_index) const;
    void ExtractIndices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData) const;
    void ExtractMaterials(FbxNode* Node, FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData) const;
    void UpdateBoundingBox(FSkeletalMeshRenderData* MeshData) const;

    FSkeletalMeshRenderData* GetSkeletalMesh(FString FilePath);

private:
    bool bInitialized = false;
    
    FbxManager* FbxManager = nullptr;

    TMap<FName, FSkeletalMeshRenderData*> SkeletalMeshData;
};
