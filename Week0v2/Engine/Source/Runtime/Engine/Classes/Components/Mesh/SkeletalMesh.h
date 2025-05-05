#pragma once
#include "FBX/FBXDefine.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeletalMesh : public UObject
{
    DECLARE_CLASS(USkeletalMesh, UObject)
public:
    USkeletalMesh() = default;
    virtual ~USkeletalMesh() override = default;

    FSkeletalMeshRenderData* GetRenderData() const { return SkeletalMeshRenderData; }
    const TArray<FMaterialSlot*>& GetMaterials() const { return MaterialSlots; }
    uint32 GetMaterialIndex(FName MaterialSlotName) const;
    void GetUsedMaterials(TArray<UMaterial*>& Out) const;

    void SetData(FSkeletalMeshRenderData* renderData);
    
    void UpdateBoneHierarchy() const;
private:
    
    FSkeletalMeshRenderData* SkeletalMeshRenderData = nullptr;
    TArray<FMaterialSlot*> MaterialSlots;

    void UpdateChildBones(int ParentIndex) const;
};
