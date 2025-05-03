#pragma once
#include "ReferenceSkeleton.h"
#include "SkinnedAsset.h"
#include "Animation/Skeleton.h"

class FSkeletalMeshRenderData;

class USkeletalMesh : public USkinnedAsset
{
    DECLARE_CLASS(USkeletalMesh, USkinnedAsset)
public:
    USkeletalMesh() = default;
    virtual ~USkeletalMesh() override;

public:
    FSkeletalMeshRenderData* GetSkeletalMeshRenderData() const;
    void SetSkeletalMeshRenderData(std::unique_ptr<FSkeletalMeshRenderData>&& InSkeletalMeshRenderData);

    
    /** USkinnedAsset interface. */
    virtual FReferenceSkeleton& GetRefSkeleton() override { return RefSkeleton; }

    /** USkinnedAsset interface. */
    virtual const FReferenceSkeleton& GetRefSkeleton() const override { return RefSkeleton; }

    void SetRefSkeleton(const FReferenceSkeleton& InRefSkeleton) { RefSkeleton = InRefSkeleton; }

private:

    FReferenceSkeleton RefSkeleton;


    std::unique_ptr<FSkeletalMeshRenderData> SkeletalMeshRenderData;





    // TODO 애니메이션 추가 시 USkeleton을 추가.
    


    // Please do not access this member directly; use USkeletalMesh::GetSkeleton() or USkeletalMesh::SetSkeleton().   
    // USkeleton* Skeleton;
};
