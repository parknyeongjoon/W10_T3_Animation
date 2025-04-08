#include "FireBallActor.h"

AFireBallActor::AFireBallActor()
{
    FireBallComponent = AddComponent<UFireBallComponent>();
}

AFireBallActor::AFireBallActor(const AFireBallActor* Other)
{
}
