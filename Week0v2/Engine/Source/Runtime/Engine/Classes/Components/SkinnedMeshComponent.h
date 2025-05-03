#pragma once
#include "PrimitiveComponents/MeshComponents/MeshComponent.h"

class USkinnedAsset;

class USkinnedMeshComponent : public UMeshComponent
{
    DECLARE_CLASS(USkinnedMeshComponent, UMeshComponent)
public:
    USkinnedMeshComponent() = default;
    virtual ~USkinnedMeshComponent() override = default;

    // UObject* Duplicate() const override;
    // void DuplicateSubObjects(const UObject* Source) override;
    // void PostDuplicate() override;
    // void TickComponent(float DeltaTime) override;

    uint32 GetNumMaterials() const override { return 0; }
    UMaterial* GetMaterial(uint32 ElementIndex) const override;
    uint32 GetMaterialIndex(FName MaterialSlotName) const override;
    UMaterial* GetMaterialByName(FName MaterialSlotName) const override;
    TArray<FName> GetMaterialSlotNames() const override;
    void SetMaterial(uint32 ElementIndex, UMaterial* Material) override;
    void SetMaterialByName(FName MaterialSlotName, UMaterial* Material) override;
    void GetUsedMaterials(TArray<UMaterial*>& Out) const override;
    
    // virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    //
    // std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    // virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    // virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;


    /** 
 * Change the SkeletalMesh that is rendered for this Component. Will re-initialize the animation tree etc. 
 *
 * @param NewMesh New mesh to set for this component
 * @param bReinitPose Whether we should keep current pose or reinitialize.
 */
    // UE_DEPRECATED(5.1, "Use USkeletalMeshComponent::SetSkeletalMesh() or SetSkinnedAssetAndUpdate() instead.")
    virtual void SetSkeletalMesh(class USkeletalMesh* NewMesh, bool bReinitPose = true);


    USkinnedAsset* GetSkinnedAsset() const;

private:
    USkinnedAsset* SkinnedAsset;
};
