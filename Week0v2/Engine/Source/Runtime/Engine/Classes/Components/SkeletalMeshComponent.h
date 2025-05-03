#pragma once
#include "SkinnedMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/Casts.h"

class USkeletalMeshComponent : public USkinnedMeshComponent
{
    DECLARE_CLASS(USkeletalMeshComponent, USkinnedMeshComponent)
public:
    USkeletalMeshComponent() = default;
    virtual ~USkeletalMeshComponent() override = default;

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

    // std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    // virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    // virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

    // 내부적으로는 메쉬 변경 → 리소스 언로드 및 리로드 → 포즈 재설정 등의 과정을 수행합니다.
    virtual void SetSkeletalMesh(class USkeletalMesh* NewMesh, bool bReinitPose = true) override;  // SetSkeletalMesh may remain and become a UFUNCTION but lose its virtual after it is removed from the SkinnedMeshComponent API

    void SetSkeletalMeshAsset(USkeletalMesh* NewMesh) { SetSkeletalMesh(NewMesh, false); }
    USkeletalMesh* GetSkeletalMeshAsset() const { return Cast<USkeletalMesh>(GetSkinnedAsset()); }

    /**
     *  Set the new asset to render and update this component (this function is identical to SetSkeletalMesh).
     *  The USkinnedAsset pointer is first cast to a USkeletalMesh, therefore this function will only set assets of type USkeletalMesh.
     * 
     *  @param InSkinnedAsset The new asset.
     *  @param bReinitPose    Whether to re-initialize the animation.
     */
    ENGINE_API virtual void SetSkinnedAssetAndUpdate(class USkinnedAsset* InSkinnedAsset, bool bReinitPose = true) override;
    
    static FVector GetSkinnedVertexPosition(USkeletalMeshComponent* Component, int32 VertexIndex, const FSkeletalMeshLODRenderData& Model, const FSkinWeightVertexBuffer& SkinWeightBuffer);
    static FVector GetSkinnedVertexPosition(USkeletalMeshComponent* Component, int32 VertexIndex, const FSkeletalMeshLODRenderData& Model, const FSkinWeightVertexBuffer& SkinWeightBuffer, TArray<FMatrix44f>& CachedRefToLocals);
    static void ComputeSkinnedPositions(USkeletalMeshComponent* Component, TArray<FVector> & OutPositions, TArray<FMatrix>& CachedRefToLocals, const FSkeletalMeshLODRenderData& Model, const FSkinWeightVertexBuffer& SkinWeightBuffer);

private:
    USkeletalMesh* SkeletalMeshAsset;
};
