#pragma once
#include "ParticleModule.h"

class UParticleModuleColor : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleColor, UParticleModule)

public:
    UParticleModuleColor() {}
    // 파티클의 색상을 설정하는 함수
    void SetColor(const FLinearColor& InColor) { Color = InColor; }
    // 파티클의 색상을 가져오는 함수

    FLinearColor GetColor() const { return Color; }

    UPROPERTY
    (EditAnywhere,
    FLinearColor,
    Color,
    = FLinearColor())
};