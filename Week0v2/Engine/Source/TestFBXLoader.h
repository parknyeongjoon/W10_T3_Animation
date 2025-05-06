#pragma once
#include <fbxsdk.h>

#include "Components/Mesh/SkeletalMesh.h"
#include "Components/Mesh/StaticMesh.h"
#include "Container/Map.h"
#include "FBX/FBXDefine.h"
#include "UObject/NameTypes.h"

class TestFBXLoader
{
public:
    static bool InitFBXManager();
    static FSkeletalMeshRenderData* ParseFBX(const FString& FilePath);
    static void ExtractFBXMeshData(const FbxScene* Scene, FSkeletalMeshRenderData* MeshData);
    static void ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData);
    static void ExtractVertices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData);
    static void ExtractNormals(FbxMesh* Mesh, FSkeletalMeshRenderData* RenderData, int BaseVertexIndex);
    static void ExtractUVs(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractTangents(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractSkinningData(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ProcessSkinning(FbxSkin* skin, FSkeletalMeshRenderData* mesh_data, int base_vertex_index);
    static void ExtractIndices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData);
    static void ExtractMaterials(FbxNode* Node, FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData);
    static void UpdateBoundingBox(FSkeletalMeshRenderData* MeshData);
    static FObjMaterialInfo ConvertFbxToObjMaterialInfo(FbxSurfaceMaterial* FbxMat, const FString& BasePath = TEXT(""));
    static UMaterial* CreateMaterial(const FObjMaterialInfo& materialInfo);

    FSkeletalMeshRenderData* GetSkeletalMeshData(FString FilePath);
    static USkeletalMesh* CreateSkeletalMesh(const FString& FilePath);
    static USkeletalMesh* GetSkeletalMesh(const FString& FilePath);
    static const TMap<FString, USkeletalMesh*>& GetSkeletalMeshes() { return SkeletalMeshMap;}

private:
    static bool bInitialized;
    
    static FbxManager* FbxManager;

    static TMap<FName, FSkeletalMeshRenderData*> SkeletalMeshData;
    static TMap<FString, USkeletalMesh*> SkeletalMeshMap;
    static TMap<FString, UMaterial*> MaterialMap;
};
