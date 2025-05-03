#include "MaterialManager.h"

#include "MaterialInfo.h"

UMaterial* FMaterialManager::CreateMaterial(const FMaterialInfo& MaterialInfo)
{
    if (MaterialMap[MaterialInfo.MaterialName] != nullptr)
        return MaterialMap[MaterialInfo.MaterialName];

    UMaterial* newMaterial = FObjectFactory::ConstructObject<UMaterial>();
    newMaterial->SetMaterialInfo(MaterialInfo);
    MaterialMap.Add(MaterialInfo.MaterialName, newMaterial);
    return newMaterial;
}

UMaterial* FMaterialManager::GetMaterial(const FString& name)
{
    if (MaterialMap.Contains(name))
    {
        return MaterialMap[name];
    }
    return nullptr;
}