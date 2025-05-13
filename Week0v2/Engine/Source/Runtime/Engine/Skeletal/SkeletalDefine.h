#pragma once
#include "Define.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "Math/Matrix.h"
#include "Math/Quat.h"
#include "Math/Vector.h"

#pragma region Skeletal;
struct FBone
{
    FString BoneName;
    FMatrix SkinningMatrix;
    FMatrix InverseBindPoseMatrix;
    FMatrix GlobalTransform;
    FMatrix LocalTransform;
    int ParentIndex;

    void Serialize(FArchive& Ar) const
    {
        Ar << BoneName
            << SkinningMatrix
            << InverseBindPoseMatrix
            << GlobalTransform
            << LocalTransform
            << ParentIndex;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> BoneName
            >> SkinningMatrix
            >> InverseBindPoseMatrix
            >> GlobalTransform
            >> LocalTransform
            >> ParentIndex;
    }
};

struct FBoneNode
{
    FString BoneName;
    int BoneIndex;             // Index in the Bones array
    TArray<int> ChildIndices;  // Indices of child bones

    void Serialize(FArchive& Ar) const
    {
        Ar << BoneName << BoneIndex << ChildIndices;
    }
    
    void Deserialize(FArchive& Ar)
    {
        Ar >> BoneName >> BoneIndex >> ChildIndices;
    }
};

struct FSkeletalVertex
{
    FVector4 Position;
    FVector Normal;
    FVector4 Tangent;
    FVector2D TexCoord;
    int32 BoneIndices[4];
    float BoneWeights[4];

    void SkinningVertex(const TArray<FBone>& bones);

    void Serialize(FArchive& Ar) const
    {
        Ar << Position << Normal << Tangent << TexCoord;
        Ar << BoneIndices[0] << BoneIndices[1] << BoneIndices[2] << BoneIndices[3];
        Ar << BoneWeights[0] << BoneWeights[1] << BoneWeights[2] << BoneWeights[3];
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> Position >> Normal >> Tangent >> TexCoord;
        Ar >> BoneIndices[0] >> BoneIndices[1] >> BoneIndices[2] >> BoneIndices[3];
        Ar >> BoneWeights[0] >> BoneWeights[1] >> BoneWeights[2] >> BoneWeights[3];
    }
    
private:
    FVector SkinVertexPosition(const TArray<FBone>& bones) const;
};


struct FRefSkeletal
{
    // Tree structure for bones
    FString Name;
    TArray<FSkeletalVertex> RawVertices;
    TArray<FBone> RawBones;
    TArray<FBoneNode> BoneTree;
    TArray<int> RootBoneIndices;  // Indices of root bones (no parents)
    TMap<FString, int> BoneNameToIndexMap;  // For quick lookups
    TArray<UMaterial*> Materials;
    TArray<FMaterialSubset> MaterialSubsets;

    void Serialize(FArchive& Ar) const;

    void Deserialize(FArchive& Ar);
};

struct FSkeletalMeshRenderData
{
    // @todo PreviewName과 FilePath 분리하기
    FString Name = "Empty";
    TArray<FSkeletalVertex> Vertices;
    TArray<uint32> Indices;
    TArray<FBone> Bones;
    FBoundingBox BoundingBox;
    ID3D11Buffer* VB = nullptr;
    ID3D11Buffer* IB = nullptr;

    void Serialize(FArchive& Ar) const
    {
        Ar << Name;
        Ar << Vertices;
        Ar << Indices;
        Ar << Bones;
        Ar << BoundingBox;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> Name >> Vertices >> Indices >> Bones >> BoundingBox;
    }
};
#pragma endregion