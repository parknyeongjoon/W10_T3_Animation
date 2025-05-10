#pragma once
#include "UObject/Object.h"

enum EAnimState
{
    AS_Idle,
    AS_Work,
    AS_Run,
    AS_Fly
};

class UAnimationStateMachine :
    public UObject
{
};

