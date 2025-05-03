#pragma once
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"

struct FVertexSimple;
struct FVector;


struct FbxMeshInfo
{
    FString Name;
    uint64 UniqueId;
    int32 FaceNum;
    int32 VertexNum;
    bool bTriangulated;
    int32 MaterialNum;
    bool bIsSkelMesh;
    FString SkeletonRoot;
    int32 SkeletonElemNum;
    FString LODGroup;
    int32 LODLevel;
    int32 MorphNum;
};

//Node use to store the scene hierarchy transform will be relative to the parent
struct FbxNodeInfo
{
    FString ObjectName;
    uint64 UniqueId;
    FbxAMatrix Transform;
    FbxVector4 RotationPivot;
    FbxVector4 ScalePivot;
	
    FString AttributeName;
    uint64 AttributeUniqueId;
    FString AttributeType;

    FString ParentName;
    uint64 ParentUniqueId;
};

struct FbxSceneInfo
{
    // data for static mesh
    int32 NonSkinnedMeshNum;
	
    //data for skeletal mesh
    int32 SkinnedMeshNum;

    // common data
    int32 TotalGeometryNum;
    int32 TotalMaterialNum;
    int32 TotalTextureNum;
	
    TArray<FbxMeshInfo> MeshInfo;
    TArray<FbxNodeInfo> HierarchyInfo;
	
    /* true if it has animation */
    bool bHasAnimation;
    bool bHasAnimationOnSkeletalMesh;
    double FrameRate;
    double TotalTime;

    void Reset()
    {
        NonSkinnedMeshNum = 0;
        SkinnedMeshNum = 0;
        TotalGeometryNum = 0;
        TotalMaterialNum = 0;
        TotalTextureNum = 0;
        MeshInfo.Empty();
        HierarchyInfo.Empty();
        bHasAnimation = false;
        bHasAnimationOnSkeletalMesh = false;
        FrameRate = 0.0;
        TotalTime = 0.0;
    }
};

class FFbxImporter
{
public:
    // Obj Parsing (*.obj to FObjInfo)
    static bool ParseOBJ(const FString& FilePath, FbxSceneInfo& OutFbxSceneInfo);
    
    // Material Parsing (*.obj to MaterialInfo)
    static bool ParseMaterial(FbxSceneInfo& OutObjInfo, Fbx::FSkeletalMeshRenderData& OutSkeletalMesh);
    
    // Convert the Raw data to Cooked data (FSkeletalMeshRenderData)
    static bool ConvertToStaticMesh(const FbxSceneInfo& RawData, Fbx::FSkeletalMeshRenderData& OutSkeletalMesh);
    
    static bool CreateTextureFromFile(const FString& Filename);

    static void ComputeBoundingBox(const TArray<FVertexSimple>& InVertices, FVector& OutMinVector, FVector& OutMaxVector);

private:
    
};


class UMaterial;
class USkeleton;
class FSkeletalMeshRenderData;
struct FFbxMaterialInfo;

class FFbxManager
{
public:
    // TODO 관리 및 생성 - 수정 (StaticMesh 관리 및 생성 참고) AssetManager로?
    static FSkeletalMeshRenderData* CreateSkeletalMeshAsset(const FString& FilePath);
    static FSkeletalMeshRenderData* GetSkeletalMeshAsset(const FString& FilePath);


private:
    static bool LoadSkeletalMesh(const FString& FilePath, FSkeletalMeshRenderData& OutSkeletonMeshRenderData);
    static bool LoadSkeletalMeshFromBinary(const FWString& FilePath, FSkeletalMeshRenderData& OutSkeletonMeshRenderData);
    // static USkeleton* GetSkeleton(const FString& FilePath);
    
    // static int GetSkeletonNum() { return SkeletonMap.Num(); }

    // Execute -> Check Contents Files -> 등록만 (USkeleton으로 만들지 않음) -> 필요한 경우 Load, 
    //                                 
private:
    // static USkeleton* CreateSkeleton(const FString& FilePath);

    
    // TODO Material Mesh 통합 관리
    // static TMap<FString, USkeleton*> SkeletonMap;
    static TMap<FString, FSkeletalMeshRenderData*> FbxSkeletonMeshMap;
};