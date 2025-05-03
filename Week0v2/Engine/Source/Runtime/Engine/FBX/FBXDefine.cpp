#include "FBXDefine.h"

FVector FSkeletalVertex::SkinVertexPosition(const TArray<FBone>& bones) const
{
    FVector result = {0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 4; ++i) {
        int boneIndex = boneIndices[i];
        float weight = boneWeights[i];

        if (weight > 0.0f && boneIndex >= 0 && boneIndex < bones.Num()) {
            const FMatrix& skinMat = bones[boneIndex].SkinningMatrix;
            FVector transformed = skinMat.TransformPosition(position);
            result = result + (transformed * weight);
        }
    }

    return result;
}
