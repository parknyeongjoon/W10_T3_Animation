#pragma once
#include "Define.h"
#include "Container/Array.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

struct FBone;
struct FFBXVertex
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
    FMatrix skinningMatrix;
};

struct FFBXMeshData
{
    TArray<FFBXVertex> vertices;
    TArray<UINT32> indices;
    TArray<UMaterial*> materials;
    TArray<FMaterialSubset> materialSubsets;
    FBoundingBox boundingBox;
};

struct FFBXAnimationData
{
    TArray<FBone> bones;
};
