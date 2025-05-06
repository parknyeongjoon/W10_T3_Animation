#include "FBXDefine.h"

#include "TestFBXLoader.h"

FVector FSkeletalVertex::SkinVertexPosition(const TArray<FBone>& bones) const
{
    FVector result = {0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 4; ++i) {
        int boneIndex = BoneIndices[i];
        float weight = BoneWeights[i];

        if (weight > 0.0f && boneIndex >= 0 && boneIndex < bones.Num()) {
            const FMatrix& SkinMat = bones[boneIndex].SkinningMatrix;
            FVector transformed = SkinMat.TransformPosition(Position.xyz());
            result = result + (transformed * weight);
        }
    }

    return result;
}

void FSkeletalVertex::SkinningVertex(const TArray<FBone>& bones)
{
    Position = FVector4(SkinVertexPosition(bones), 1.0f);
}
