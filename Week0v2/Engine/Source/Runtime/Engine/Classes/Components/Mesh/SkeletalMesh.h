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

    FSkeletalMeshRenderData& GetRenderData() { return SkeletalMeshRenderData; }
    FRefSkeletal* GetRefSkeletal() const { return RefSkeletal;}
    const TArray<FMaterialSlot*>& GetMaterials() const { return MaterialSlots; }
    uint32 GetMaterialIndex(FName MaterialSlotName) const;
    void GetUsedMaterials(TArray<UMaterial*>& Out) const;
    void SetData(const FString& FilePath);

    void SetData(const FSkeletalMeshRenderData& InRenderData, FRefSkeletal* InRefSkeletal);
    
    void UpdateBoneHierarchy();
    // 정점 스키닝을 업데이트하는 함수
    void UpdateSkinnedVertices();

    // 버텍스 버퍼를 업데이트하는 함수
    void UpdateVertexBuffer();
    void RotateBoneByName(const FString& BoneName, float AngleInDegrees, const FVector& RotationAxis);
    int FindBoneIndexByName(const FString& BoneName) const;
    void ApplyRotationToBone(int BoneIndex, float AngleInDegrees, const FVector& RotationAxis);

    USkeletalMesh* Duplicate(UObject* InOuter) override;
private:
    
    FSkeletalMeshRenderData SkeletalMeshRenderData;
    FRefSkeletal* RefSkeletal = nullptr;
    TArray<FMaterialSlot*> MaterialSlots;

    TArray<FMatrix> OriginalLocalTransforms;
    TArray<FMatrix> OriginalGlobalMatrices;
    TArray<FVector4> OriginalVertexPositions;

    void UpdateChildBones(int ParentIndex);
    void ResetToOriginalPose();
};
