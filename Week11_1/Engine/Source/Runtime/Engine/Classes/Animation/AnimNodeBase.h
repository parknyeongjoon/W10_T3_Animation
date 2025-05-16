#pragma once
#include "Container/Array.h"
#include "Math/Transform.h"
#include "CoreUObject/UObject/NameTypes.h"
#include "Container/Map.h"

struct FCompactPose
{
    TArray<FTransform> BoneTransforms;

    FTransform GetBoneTransform(int32 BoneIndex) const
    {
        return BoneTransforms[BoneIndex];
    }

    void SetBoneTransform(int32 BoneIndex, const FTransform& Transform)
    {
        BoneTransforms[BoneIndex] = Transform;
    }
};

struct FBlendedCurve
{
    TMap<FName, float> CurveValues;

    void SetCurveValue(const FName& CurveName, float Value)
    {
        CurveValues.Emplace(CurveName, Value);
    }

    // 커브 값 가져오기
    float GetCurveValue(const FName& CurveName) const
    {
        return CurveValues.Contains(CurveName) ? CurveValues[CurveName] : 0.0f;
    }

    // 블렌딩 처리
    void Blend(const FBlendedCurve& Other, float Alpha)
    {
        for (const auto& Pair : Other.CurveValues)
        {
            if (CurveValues.Contains(Pair.Key))
            {
                CurveValues[Pair.Key] = FMath::Lerp(CurveValues[Pair.Key], Pair.Value, Alpha);
            }
            else
            {
                CurveValues.Emplace(Pair.Key, Pair.Value);
            }
        }
    }
};

struct FAnimationBaseContext
{
};

struct FPoseContext : public FAnimationBaseContext
{
    FCompactPose Pose;
    FBlendedCurve Curve;
    float Alpha;

    FPoseContext()
        : Alpha(1.0f) {
    }

    FPoseContext(const FPoseContext& Other)
        : Pose(Other.Pose), Curve(Other.Curve), Alpha(Other.Alpha) {
    }

};