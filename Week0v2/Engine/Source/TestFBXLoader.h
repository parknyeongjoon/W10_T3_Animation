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

    static void ExtractFBXMeshData(const FbxScene* Scene, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal);
    static void ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal);
    static void ExtractVertices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal);
    static void ExtractNormals(FbxMesh* Mesh, FSkeletalMeshRenderData* RenderData, int BaseVertexIndex);
    static void ExtractUVs(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractTangents(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractSkinningData(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal, int BaseVertexIndex);
    static void ProcessSkinning(FbxSkin* skin, FSkeletalMeshRenderData* mesh_data, FRefSkeletal* RefSkeletal, int base_vertex_index);
    static void ExtractIndices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractMaterials(FbxNode* Node, FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal, int BaseIndexOffset);
    static void UpdateBoundingBox(FSkeletalMeshRenderData* MeshData);

    static FSkeletalMeshRenderData* GetSkeletalRenderData(FString FilePath);
    static FSkeletalMeshRenderData GetCopiedSkeletalRenderData(FString FilePath);
    static TMap<FName, FSkeletalMeshRenderData*> GetAllSkeletalMeshes() { return SkeletalMeshData; }
    static FRefSkeletal* GetRefSkeletal(FString FilePath);
    static TMap<FName, FRefSkeletal*> GetAllRefSkeletals() { return RefSkeletalData; }
    static void UpdateBoundingBox(FSkeletalMeshRenderData MeshData);
    static FObjMaterialInfo ConvertFbxToObjMaterialInfo(FbxSurfaceMaterial* FbxMat, const FString& BasePath = TEXT(""));

    static USkeletalMesh* GetSkeletalMeshData(FString FilePath) { return SkeletalMeshMap[FilePath]; }
    static USkeletalMesh* CreateSkeletalMesh(const FString& FilePath);
    static USkeletalMesh* GetSkeletalMesh(const FString& FilePath);
    static const TMap<FString, USkeletalMesh*>& GetSkeletalMeshes() { return SkeletalMeshMap;}

    static void AddVertexFromControlPoint(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int ControlPointIndex);

private:
    inline static bool bInitialized = false;
    
    inline static FbxManager* FbxManager;

    inline static TMap<FName, FSkeletalMeshRenderData*> SkeletalMeshData;
    inline static TMap<FString, USkeletalMesh*> SkeletalMeshMap;
    inline static TMap<FName, FRefSkeletal*> RefSkeletalData;
};
