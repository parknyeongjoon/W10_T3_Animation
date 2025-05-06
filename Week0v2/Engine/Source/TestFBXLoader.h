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

    static FSkeletalMeshRenderData* GetSkeletalRenderData(FString FilePath);
    static FSkeletalMeshRenderData GetCopiedSkeletalRenderData(FString FilePath);
    static TMap<FName, FSkeletalMeshRenderData*> GetAllSkeletalMeshes() { return SkeletalMeshData; }
    static FRefSkeletal* GetRefSkeletal(FString FilePath);
    static TMap<FName, FRefSkeletal*> GetAllRefSkeletals() { return RefSkeletalData; }
    static void UpdateBoundingBox(FSkeletalMeshRenderData MeshData);
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

    inline static TMap<FName, FRefSkeletal*> RefSkeletalData;
};
