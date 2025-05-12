#pragma once
#include "AnimNodeBase.h"

class FAnimationRuntime
{
public:
    static void BlendTwoPosesTogether(
        const FCompactPose& FromPose,
        const FCompactPose& ToPose,
        const FBlendedCurve& FromCurve,
        const FBlendedCurve& ToCurve,
        const float BlendAlpha,
        /*out*/ FCompactPose& OutPose,
        /*out*/ FBlendedCurve& OutCurve);


};

