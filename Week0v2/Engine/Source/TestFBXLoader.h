#pragma once
#include <fbxsdk.h>

#include "Components/Mesh/SkeletalMesh.h"
#include "Components/Mesh/StaticMesh.h"
#include "Container/Map.h"
#include "FBX/FBXDefine.h"

class TestFBXLoader
{
public:
    static bool InitFBXManager();
    static FSkeletalMeshRenderData* ParseFBX(const FString& FilePath);

    static void ExtractFBXMeshData(const FbxScene* Scene, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal);
    static void ExtractBoneFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal);
    static void ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal);
    static void ExtractVertices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal);
    static void ExtractNormal(FbxMesh* Mesh, FSkeletalVertex& Vertex, int PolygonIndex, int VertexIndex);
    static void ExtractUV(FbxMesh* Mesh, FSkeletalVertex& Vertex, int PolygonIndex, int VertexIndex);
    static void ExtractTangent(FbxMesh* Mesh, FSkeletalVertex& Vertex, int PolygonIndex, int VertexIndex);
    static void ExtractSkinningData(FbxMesh* Mesh, FRefSkeletal* RefSkeletal);
    static void StoreWeights(FbxMesh* Mesh, FSkeletalVertex& Vertex, int PolygonIndex, int VertexIndex);
    static void StoreVertex(FSkeletalVertex& vertex, FSkeletalMeshRenderData* MeshData);
    static void ProcessSkinning(FbxSkin* skin, FSkeletalMeshRenderData* mesh_data, FRefSkeletal* RefSkeletal, int base_vertex_index);
    static void ExtractIndices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex);
    static void ExtractMaterials(FbxNode* Node, FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal, int BaseIndexOffset);
    static void UpdateBoundingBox(FSkeletalMeshRenderData* MeshData);

    static FSkeletalMeshRenderData* GetSkeletalRenderData(FString FilePath);
    static FSkeletalMeshRenderData GetCopiedSkeletalRenderData(FString FilePath);
    
    static FRefSkeletal* GetRefSkeletal(FString FilePath);
    static TMap<FName, FRefSkeletal*> GetAllRefSkeletals() { return RefSkeletalData; }
    
    static void UpdateBoundingBox(FSkeletalMeshRenderData MeshData);
    
    static FObjMaterialInfo ConvertFbxToObjMaterialInfo(FbxSurfaceMaterial* FbxMat, const FString& BasePath = TEXT(""));

    static USkeletalMesh* CreateSkeletalMesh(const FString& FilePath);
    static TMap<FName, FSkeletalMeshRenderData*> GetAllSkeletalMeshes() { return SkeletalMeshData; }
    static USkeletalMesh* GetSkeletalMesh(const FString& FilePath);
    static USkeletalMesh* GetSkeletalMeshData(FString FilePath) { return SkeletalMeshMap[FilePath]; }
    static const TMap<FString, USkeletalMesh*>& GetSkeletalMeshes() { return SkeletalMeshMap;}

    static FSkeletalVertex& GetVertexFromControlPoint(FbxMesh* Mesh, int PolygonIndex, int VertexIndex);
    static bool IsTriangulated(FbxMesh* Mesh);

private:
    inline static FbxManager* FbxManager;

    inline static TMap<FName, FSkeletalMeshRenderData*> SkeletalMeshData;
    inline static TMap<FString, USkeletalMesh*> SkeletalMeshMap;
    inline static TMap<FName, FRefSkeletal*> RefSkeletalData;
    
    inline static TMap<FString, uint32> IndexMap;
    struct FBoneWeightInfo
    {
        int BoneIndex;
        float BoneWeight;
    };
    inline static TMap<uint32, TArray<FBoneWeightInfo>> SkinWeightMap;
};
