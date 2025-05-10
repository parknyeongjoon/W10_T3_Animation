#pragma once
#include "UObject/Object.h"
class UAnimInstance :
    public UObject
{
    void TriggerAnimNotifies(float DeltaSeconds);
};

