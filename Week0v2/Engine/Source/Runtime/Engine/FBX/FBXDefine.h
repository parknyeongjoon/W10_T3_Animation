#pragma once
#include "Define.h"
#include "Container/Array.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

struct FBone;
struct FSkeletalVertex
{
    FVector position;
    FVector normal;
    FVector2D texCoord;
    FVector tangent;
    
    int boneIndices[4];
    float boneWeights[4];

    FVector SkinVertexPosition(const TArray<FBone>& bones) const;
};

struct FBone
{
    FMatrix SkinningMatrix;
};

struct FSkeletalMeshRenderData
{
    FString Name;
    TArray<FSkeletalVertex> Vertices;
    TArray<UINT32> Indices;
    TArray<UMaterial*> Materials;
    TArray<FMaterialSubset> MaterialSubsets;
    FBoundingBox BoundingBox;
};

struct FSkeletalAnimationData
{
    TArray<FBone> Bones;
};
