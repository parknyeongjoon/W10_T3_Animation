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

    FVector SkinVertexPosition(const TArray<FBone>& bones) const;
    void TranslateVertexByBone(const TArray<FBone>& bones);
};


struct FSkeletalMeshRenderData
{
    FString Name;
    TArray<FSkeletalVertex> Vertices;
    TArray<uint32> Indices;
    TArray<UMaterial*> Materials;
    TArray<FMaterialSubset> MaterialSubsets;
    FBoundingBox BoundingBox;

    TArray<FBone> Bones;
    // Tree structure for bones
    TArray<FBoneNode> BoneTree;
    TArray<int> RootBoneIndices;  // Indices of root bones (no parents)
    TMap<FString, int> BoneNameToIndexMap;  // For quick lookups
};
