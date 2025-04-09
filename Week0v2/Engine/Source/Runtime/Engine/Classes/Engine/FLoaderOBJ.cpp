#include "FLoaderOBJ.h"
#include "UObject/ObjectFactory.h"
#include "Components/Material/Material.h"
#include "Components/Mesh/StaticMesh.h"

UMaterial* FManagerOBJ::CreateMaterial(FObjMaterialInfo materialInfo)
{
    if (materialMap[materialInfo.MTLName] != nullptr)
        return materialMap[materialInfo.MTLName];

    UMaterial* newMaterial = FObjectFactory::ConstructObject<UMaterial>();
    newMaterial->SetMaterialInfo(materialInfo);
    materialMap.Add(materialInfo.MTLName, newMaterial);
    return newMaterial;
}

UMaterial* FManagerOBJ::GetMaterial(FString name)
{
    return materialMap[name];
}

UStaticMesh* FManagerOBJ::CreateStaticMesh(FString filePath)
{

    OBJ::FStaticMeshRenderData* staticMeshRenderData = FManagerOBJ::LoadObjStaticMeshAsset(filePath);

    if (staticMeshRenderData == nullptr) return nullptr;

    UStaticMesh* staticMesh = GetStaticMesh(staticMeshRenderData->ObjectName);
    if (staticMesh != nullptr) {
        return staticMesh;
    }

    staticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
    staticMesh->SetData(staticMeshRenderData);

    staticMeshMap.Add(staticMeshRenderData->ObjectName, staticMesh);

    return staticMesh;
}

UStaticMesh* FManagerOBJ::GetStaticMesh(FWString name)
{
    return staticMeshMap[name];
}
