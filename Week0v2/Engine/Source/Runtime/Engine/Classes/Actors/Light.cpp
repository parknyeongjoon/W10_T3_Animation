#include "Light.h"
#include "Components/UBillboardComponent.h"

ALight::ALight()
{
    Texture2D = AddComponent<UBillboardComponent>();
    Texture2D->SetTexture(L"Assets/Texture/spotLight.png");
}

ALight::ALight(const ALight& Other)
{

}

ALight::~ALight()
{
}

