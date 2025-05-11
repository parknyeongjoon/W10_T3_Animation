#pragma once
#include <fbxsdk.h>

#include "Animation/AnimTypes.h"
#include "Components/Mesh/SkeletalMesh.h"
#include "Components/Mesh/StaticMesh.h"
#include "Container/Map.h"
#include "Skeletal/SkeletalDefine.h"

class UAnimDataModel;

class TestFBXLoader
{
public:
    static bool InitFBXManager();
    static FSkeletalMeshRenderData* ParseFBX(const FString& FilePath);

    static USkeletalMesh* CreateSkeletalMesh(const FString& FilePath);
    static USkeletalMesh* GetSkeletalMesh(const FString& FilePath);
    static const TMap<FString, USkeletalMesh*>& GetSkeletalMeshes() { return SkeletalMeshMap;}
    
    static FSkeletalMeshRenderData* GetSkeletalRenderData(FString FilePath);
    static FSkeletalMeshRenderData GetCopiedSkeletalRenderData(FString FilePath);
    static TMap<FName, FSkeletalMeshRenderData*> GetAllSkeletalMeshData() { return SkeletalMeshData; }
    
    static FRefSkeletal* GetRefSkeletal(FString FilePath);
    static TMap<FName, FRefSkeletal*> GetAllRefSkeletal() { return RefSkeletalData; }
    
    static void UpdateBoundingBox(FSkeletalMeshRenderData& MeshData);
private:
    // Skeletal
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
    
    static FObjMaterialInfo ConvertFbxToObjMaterialInfo(FbxSurfaceMaterial* FbxMat, const FString& BasePath = TEXT(""));
    static FSkeletalVertex& GetVertexFromControlPoint(FbxMesh* Mesh, int PolygonIndex, int VertexIndex);
    static bool IsTriangulated(FbxMesh* Mesh);

    
    // Anim
    static void ExtractFBXAnimData(const FbxScene* scene, const FString& FilePath);
    static void ExtractAnimClip(FbxAnimStack* AnimStack, const TArray<FbxNode*>& BoneNodes, const FString& FilePath);
    static void ExtractAnimTrack(FbxAnimLayer* AnimLayer, FbxNode* BoneNode, FRawAnimSequenceTrack& AnimTrack);

private:
    inline static FbxManager* FbxManager;

    inline static TMap<FName, FSkeletalMeshRenderData*> SkeletalMeshData;
    inline static TMap<FString, USkeletalMesh*> SkeletalMeshMap;
    inline static TMap<FName, FRefSkeletal*> RefSkeletalData;
    inline static TMap<FName, UAnimDataModel*> AnimDataMap;

    // data structure for parsing
    inline static TMap<FString, uint32> IndexMap;
    struct FBoneWeightInfo
    {
        int BoneIndex;
        float BoneWeight;
    };
    inline static TMap<uint32, TArray<FBoneWeightInfo>> SkinWeightMap;
};
