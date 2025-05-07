#pragma once
#include "Define.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UMaterial : public UObject {
    DECLARE_CLASS(UMaterial, UObject)

public:
    UMaterial() {}
    UMaterial(FObjMaterialInfo InMaterialInfo) : materialInfo(std::move(InMaterialInfo)) {}
    ~UMaterial() {}
    FObjMaterialInfo& GetMaterialInfo() { return materialInfo; }
    void SetMaterialInfo(FObjMaterialInfo value) { materialInfo = value; }

    // 색상 및 재질 속성 설정자
    void SetDiffuse(const FVector& DiffuseIn) { materialInfo.Diffuse = DiffuseIn; }
    void SetSpecular(const FVector& SpecularIn) { materialInfo.Specular = SpecularIn; }
    void SetAmbient(const FVector& AmbientIn) { materialInfo.Ambient = AmbientIn; }
    void SetEmissive(const FVector& EmissiveIn) { materialInfo.Emissive = EmissiveIn; }

    // 스칼라 속성 설정자
    void SetSpecularPower(float SpecularPowerIn) { materialInfo.SpecularScalar = SpecularPowerIn; }
    void SetOpticalDensity(float DensityIn) { materialInfo.DensityScalar = DensityIn; }
    void SetTransparency(float TransparencyIn) {
        materialInfo.TransparencyScalar = TransparencyIn;
        materialInfo.bTransparent = (TransparencyIn < 1.0f);
    }
private:
    FObjMaterialInfo materialInfo;
};
