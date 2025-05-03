#pragma once
#include "Container/Map.h"

class FString;
class UMaterial;
struct FMaterialInfo;

class FMaterialManager
{
public:

    static UMaterial* CreateMaterial(const FMaterialInfo& MaterialInfo);
    
    static UMaterial* GetMaterial(const FString& name);
    static TMap<FString, UMaterial*>& GetMaterials() { return MaterialMap; }
    static int GetMaterialNum() { return MaterialMap.Num(); }

private:
    static TMap<FString, UMaterial*> MaterialMap;
};
