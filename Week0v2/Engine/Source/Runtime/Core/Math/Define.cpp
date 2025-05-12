#include "Define.h"
#include "Runtime/Serialization/Archive.h"

void FMaterialSubset::Serialize(FArchive& Ar) const
{
    Ar << IndexStart << IndexCount << MaterialIndex << MaterialName;
}

void FMaterialSubset::Deserialize(FArchive& Ar)
{
    Ar >> IndexStart >> IndexCount >> MaterialIndex >> MaterialName;
}

void FObjMaterialInfo::Serialize(FArchive& Ar) const
{
    Ar << MTLName << bHasTexture << bTransparent;
    Ar << Diffuse << Specular << Ambient << Emissive
        << SpecularScalar << DensityScalar << TransparencyScalar << IlluminanceModel;
    Ar << DiffuseTextureName << DiffuseTexturePath
        << AmbientTextureName << AmbientTexturePath
        << SpecularTextureName << SpecularTexturePath
        << BumpTextureName << BumpTexturePath
        << AlphaTextureName << AlphaTexturePath
        << NormalTextureName << NormalTexturePath;
    Ar << NormalScale;
}

void FObjMaterialInfo::Deserialize(FArchive& Ar)
{
    Ar >> MTLName >> bHasTexture >> bTransparent;
    Ar >> Diffuse >> Specular >> Ambient >> Emissive
        >> SpecularScalar >> DensityScalar >> TransparencyScalar >> IlluminanceModel;
    Ar >> DiffuseTextureName >> DiffuseTexturePath
        >> AmbientTextureName >> AmbientTexturePath
        >> SpecularTextureName >> SpecularTexturePath
        >> BumpTextureName >> BumpTexturePath
        >> AlphaTextureName >> AlphaTexturePath
        >> NormalTextureName >> NormalTexturePath;
    Ar >> NormalScale;
}