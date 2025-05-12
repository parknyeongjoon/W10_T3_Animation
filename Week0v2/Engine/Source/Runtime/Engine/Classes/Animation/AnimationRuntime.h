#pragma once
#include "AnimNodeBase.h"

class FAnimationRuntime
{
public:
    static void BlendTwoPosesTogether(
        const FCompactPose& SourcePose1,
        const FCompactPose& SourcePose2,
        const FBlendedCurve& SourceCurve1,
        const FBlendedCurve& SourceCurve2,
        const float WeightOfPose1,
        /*out*/ FCompactPose& ResultPose,
        /*out*/ FBlendedCurve& ResultCurve);


};

