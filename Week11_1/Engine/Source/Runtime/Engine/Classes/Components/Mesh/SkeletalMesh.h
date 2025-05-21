#pragma once
#include "Skeletal/SkeletalDefine.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeleton;

class USkeletalMesh : public UObject
{
    DECLARE_CLASS(USkeletalMesh, UObject)
public:
    USkeletalMesh() = default;
    virtual ~USkeletalMesh() override = default;
    
    USkeletalMesh* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;

    FSkeletalMeshRenderData& GetRenderData() { return SkeletalMeshRenderData; }
    USkeleton* GetSkeleton() const { return Skeleton;}
    const TArray<FMaterialSlot*>& GetMaterials() const { return MaterialSlots; }
    uint32 GetMaterialIndex(FName MaterialSlotName) const;
    void GetUsedMaterials(TArray<UMaterial*>& Out) const;
    
    void SetData(const FString& FilePath);
    void SetData(const FSkeletalMeshRenderData& InRenderData, USkeleton* InSkeleton);
    
    void UpdateBoneHierarchy();
    // 정점 스키닝을 업데이트하는 함수
    void UpdateSkinnedVertices();

    // 버텍스 버퍼를 업데이트하는 함수
    void UpdateVertexBuffer();
    void RotateBoneByName(const FString& BoneName, float DeltaAngleInDegrees, const FVector& RotationAxis);
    void RotateBoneByIndex(int32 BoneIndex, float DeltaAngleInDegrees, const FVector& RotationAxis, bool bIsChildUpdate = true);
    int FindBoneIndexByName(const FString& BoneName) const;

    void ResetToOriginalPose();
private:
    
    FSkeletalMeshRenderData SkeletalMeshRenderData;
    USkeleton* Skeleton;
    TArray<FMaterialSlot*> MaterialSlots;

    void UpdateChildBones(int ParentIndex);
    void ApplyRotationToBone(int BoneIndex, float DeltaAngleInDegrees, const FVector& RotationAxis);
};
