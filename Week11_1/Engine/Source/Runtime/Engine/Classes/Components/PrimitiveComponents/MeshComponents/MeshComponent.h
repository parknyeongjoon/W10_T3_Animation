#pragma once
#include "Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Components/Material/Material.h"

class UMeshComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UMeshComponent, UPrimitiveComponent)
public:
    UMeshComponent() = default;

#pragma region Material
    virtual uint32 GetNumMaterials() const { return 0; }
    virtual UMaterial* GetMaterial(uint32 ElementIndex) const;
    virtual uint32 GetMaterialIndex(FName MaterialSlotName) const;
    virtual UMaterial* GetMaterialByName(FName MaterialSlotName) const;
    virtual TArray<FName> GetMaterialSlotNames() const;
    virtual void SetMaterial(uint32 ElementIndex, UMaterial* Material);
    virtual void SetMaterialByName(FName MaterialSlotName, class UMaterial* Material);
    virtual void GetUsedMaterials(TArray<UMaterial*>& Out) const;
#pragma endregion
protected:
    TArray<UMaterial*> OverrideMaterials;
public:
    TArray<UMaterial*>& GetOverrideMaterials() { return OverrideMaterials; }
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
};

