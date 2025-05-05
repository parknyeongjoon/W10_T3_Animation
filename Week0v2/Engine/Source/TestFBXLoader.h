#pragma once
#include <fbxsdk.h>

#include "Container/Map.h"
#include "FBX/FBXDefine.h"
#include "UObject/NameTypes.h"

class TestFBXLoader
{
public:
    bool InitFBXManager();
    static bool InitFBX(const FString& FilePath);
    
    static FSkeletalMeshRenderData* GetSkeletalMesh(FString FilePath);
    static TMap<FName, FSkeletalMeshRenderData*> GetAllSkeletalMeshes() { return SkeletalMeshData; }
private:
    static void ExtractFBXMeshData(const FbxScene* Scene, FSkeletalMeshRenderData* MeshData);
    static void ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData);
    static void ExtractVertices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData);
    static void ExtractNormals(FbxMesh* Mesh, FSkeletalMeshRenderData* RenderData, int BaseVertexIndex);
    static void ExtractUVs(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractTangents(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractSkinningData(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ProcessSkinning(FbxSkin* skin, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractIndices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData);
    static void ExtractMaterials(FbxNode* Node, FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData);
    static void UpdateBoundingBox(FSkeletalMeshRenderData* MeshData);

private:
    bool bInitialized = false;
    inline static FbxManager* FbxManager = nullptr;

   inline static TMap<FName, FSkeletalMeshRenderData*> SkeletalMeshData;
};
