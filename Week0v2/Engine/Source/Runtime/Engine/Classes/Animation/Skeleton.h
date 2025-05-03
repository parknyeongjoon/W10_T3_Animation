// #pragma once
// #include "Container/Array.h"
// #include "UObject/Object.h"
// #include "UObject/ObjectMacros.h"
//
// class USkeleton : public UObject
// {
//     DECLARE_CLASS(USkeleton, UObject)
// public:
//
//     // Skeleton에 대한 메타 정보
//     TArray<struct FBoneNode> BoneTree;
//
//     TArray<FTransform> RefLocalPoses_DEPRECATED;
//     TEnumAsByte<EAxis::Type> PreviewForwardAxis;
//     /** Reference Skeleton */
//     FReferenceSkeleton ReferenceSkeleton;
//
//     
//     /** Accessor to Reference Skeleton to make data read only */
//     const FReferenceSkeleton& GetReferenceSkeleton() const
//     {
//         return ReferenceSkeleton;
//     }
//
//     /** Accessor for the array of virtual bones on this skeleton */
//     const TArray<FVirtualBone>& GetVirtualBones() const { return VirtualBones; }
//
//     /** 
//      *	Array of named socket locations, set up in editor and used as a shortcut instead of specifying 
//      *	everything explicitly to AttachComponent in the SkeletalMeshComponent.
//      */
//     UPROPERTY()
//     TArray<TObjectPtr<class USkeletalMeshSocket>> Sockets;
//     ENGINE_API int32 GetChildBones(int32 ParentBoneIndex, TArray<int32> & Children) const;
//
//
//     /**
//  * Get the local-space ref pose for the specified retarget source.
//  * @param	RetargetSource	The name of the retarget source to find
//  * @return the transforms for the retarget source reference pose. If the retarget source is not found, this returns the skeleton's reference pose.
//  */
//     ENGINE_API const TArray<FTransform>& GetRefLocalPoses( FName RetargetSource = NAME_None ) const;
//
//     /** 
//  * Get Bone Tree Index from Reference Bone Index
//  * @param	InSkinnedAsset	SkinnedAsset for the ref bone idx
//  * @param	InRefBoneIdx	Reference Bone Index to look for - index of USkinnedAsset.RefSkeleton
//  * @return	Index of BoneTree Index
//  */
//     ENGINE_API int32 GetSkeletonBoneIndexFromMeshBoneIndex(const USkinnedAsset* InSkinnedAsset, const int32 MeshBoneIndex);
//
//     /** 
//  * Get Reference Bone Index from Bone Tree Index
//  * @param	InSkinnedAsset	SkinnedAsset for the ref bone idx
//  * @param	InBoneTreeIdx	Bone Tree Index to look for - index of USkeleton.BoneTree
//  * @return	Index of BoneTree Index
//  */
//     ENGINE_API int32 GetMeshBoneIndexFromSkeletonBoneIndex(const USkinnedAsset* InSkinnedAsset, const int32 SkeletonBoneIndex);
//
//     
//
//     virtual bool IsPostLoadThreadSafe() const override;
//     ENGINE_API virtual void PostLoad() override;
//     ENGINE_API virtual void PostDuplicate(bool bDuplicateForPIE) override;
//     ENGINE_API virtual void PostInitProperties() override;
//     ENGINE_API virtual void Serialize(FArchive& Ar) override;
//
//     /** 
//  * Create RefLocalPoses from InSkinnedAsset. Note InSkinnedAsset cannot be null and this function will assert if it is.
//  * 
//  * If bClearAll is false, it will overwrite ref pose of bones that are found in InSkelMesh
//  * If bClearAll is true, it will reset all Reference Poses 
//  * Note that this means it will remove transforms of extra bones that might not be found in this InSkinnedAsset
//  *
//  * @return true if successful. false if InSkinnedAsset wasn't compatible with the bone hierarchy
//  */
//     ENGINE_API void UpdateReferencePoseFromMesh(const USkinnedAsset* InSkinnedAsset);
//
//     /**
//  * Create Reference Skeleton From the given Mesh 
//  * 
//  * @param InSkinnedAsset	SkinnedAsset that this Skeleton is based on
//  * @param RequiredRefBones	List of required bones to create skeleton from
//  *
//  * @return true if successful
//  */
//     bool CreateReferenceSkeletonFromMesh(const USkinnedAsset* InSkinnedAsset, const TArray<int32> & RequiredRefBones);
//     
// };
