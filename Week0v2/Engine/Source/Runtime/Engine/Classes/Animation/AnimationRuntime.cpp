#include "AnimationRuntime.h"

void FAnimationRuntime::BlendTwoPosesTogether(
    const FCompactPose& FromPose,
    const FCompactPose& ToPose,
    const FBlendedCurve& FromCurve,
    const FBlendedCurve& ToCurve,
    const float BlendAlpha,
    /*out*/ FCompactPose& OutPose,
    /*out*/ FBlendedCurve& OutCurve)
{
    if (FromPose.BoneTransforms.Num() != ToPose.BoneTransforms.Num())
    {
        return;
    }
    OutPose.BoneTransforms.SetNum(FromPose.BoneTransforms.Num());
    for (int32 BoneIndex = 0; BoneIndex < FromPose.BoneTransforms.Num(); ++BoneIndex)
    {
        const FTransform& FromTransform = FromPose.BoneTransforms[BoneIndex];
        const FTransform& ToTransform = ToPose.BoneTransforms[BoneIndex];
        OutPose.BoneTransforms[BoneIndex] = FTransform::Blend(FromTransform, ToTransform, BlendAlpha);
    }

    //TODO Curve Blending
}
