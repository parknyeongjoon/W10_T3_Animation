#pragma once
#include "Define.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

struct FBone
{
    FString BoneName;
    FMatrix SkinningMatrix;
    FMatrix InverseBindPoseMatrix;
    FMatrix GlobalTransform;
    FMatrix LocalTransform;
    int ParentIndex;
};

struct FBoneNode
{
    FString BoneName;
    int BoneIndex;             // Index in the Bones array
    TArray<int> ChildIndices;  // Indices of child bones
};

struct FSkeletalVertex
{
    FVector4 Position;
    FVector Normal;
    FVector Tangent;
    FVector2D TexCoord;
    int32 BoneIndices[4];
    float BoneWeights[4];

    void SkinningVertex(const TArray<FBone>& bones);
private:
    FVector SkinVertexPosition(const TArray<FBone>& bones) const;
};


struct FRefSkeletal
{
    // Tree structure for bones
    FString Name;
    TArray<FSkeletalVertex> RawVertices;
    TArray<FBoneNode> BoneTree;
    TArray<int> RootBoneIndices;  // Indices of root bones (no parents)
    TMap<FString, int> BoneNameToIndexMap;  // For quick lookups
    TArray<UMaterial*> Materials;
    TArray<FMaterialSubset> MaterialSubsets;
};

struct FSkeletalMeshRenderData
{
    FString Name = "Empty";
    TArray<FSkeletalVertex> Vertices;
    TArray<uint32> Indices;
    TArray<FBone> Bones;
    FBoundingBox BoundingBox;
    ID3D11Buffer* VB = nullptr;
    ID3D11Buffer* IB = nullptr;
};
