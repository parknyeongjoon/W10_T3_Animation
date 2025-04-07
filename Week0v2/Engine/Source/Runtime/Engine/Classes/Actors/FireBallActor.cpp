#include "FireBallActor.h"

AFireBallActor::AFireBallActor()
{
    FireBallComponent = AddComponent<UFireBallComponent>();
}
