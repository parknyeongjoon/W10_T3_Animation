#include "SpotLightActor.h"

#include "Components/LightComponents/SpotLightComponent.h"
#include "Components/PrimitiveComponents/UBillboardComponent.h"

ASpotLightActor::ASpotLightActor()
{
    LightComponent = AddComponent<USpotLightComponent>(EComponentOrigin::Constructor);
    LightComponent->SetRelativeRotation(FRotator(0, -3.14f / 2.0f, 0));
    BillboardComponent->SetTexture(L"Assets/Texture/SpotLight_64x.png");
}
     
