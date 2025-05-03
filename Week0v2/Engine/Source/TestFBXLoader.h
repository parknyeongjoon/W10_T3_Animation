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
    void ExtractFBXMeshData(const FbxScene* Scene, FFBXMeshData* MeshData) const;
    void ExtractMeshFromNode(FbxNode* Node, FFBXMeshData* MeshData) const;
    void ExtractVertices(FbxMesh* Mesh, FFBXMeshData* MeshData) const;
    void ExtractNormals(FbxMesh* Mesh, FFBXMeshData* MeshData, int BaseVertexIndex) const;
    void ExtractUVs(FbxMesh* Mesh, FFBXMeshData* MeshData, int BaseVertexIndex) const;
    void ExtractTangents(FbxMesh* Mesh, FFBXMeshData* MeshData, int BaseVertexIndex) const;
    void ExtractIndices(FbxMesh* Mesh, FFBXMeshData* MeshData) const;
    void ExtractMaterials(FbxNode* Node, FbxMesh* Mesh, FFBXMeshData* MeshData) const;
    void UpdateBoundingBox(FFBXMeshData* MeshData) const;

private:
    bool bInitialized = false;
    
    FbxManager* FbxManager = nullptr;

    TMap<FName, FFBXMeshData*> FBXMeshData;
    TMap<FName, FFBXAnimationData*> FBXAnimationData;
};
