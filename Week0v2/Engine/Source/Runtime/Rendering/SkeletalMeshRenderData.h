#pragma once
#include "SkeletalMeshLODRenderData.h"
#include "Container/Array.h"

class USkinnedAsset;

// TODO
// LOD를 구현하지 않아 RenderData가 Tarray가 아니다.

class FSkeletalMeshRenderData
{
public:
    /** Per-LOD render data. */
    FSkeletalMeshLODRenderData RenderData;

    /** True if RHI Resources are initialized */
    bool bReadyForStreaming;

    /** Initializes rendering resources. */
    void InitResources(bool bNeedsVertexColors, TArray<UMorphTarget*>& InMorphTargets, USkinnedAsset* Owner);

    /** Releases rendering resources. */
    void ReleaseResources();

    /** Return the resource size */
    void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize);

    /** Get the estimated memory overhead of buffers marked as NeedsCPUAccess. */
    SIZE_T GetCPUAccessMemoryOverhead() const;

    /** Returns true if this resource must be skinned on the CPU for the given feature level. */
    bool RequiresCPUSkinning(ERHIFeatureLevel::Type FeatureLevel) const;

    // Serialize

    /** Returns the number of bone influences per vertex. */
    uint32 GetNumBoneInfluences() const;

    /**
    * Computes the maximum number of bones per section used to render this mesh.
    */
    int32 GetMaxBonesPerSection() const;

    
    
    bool IsInitialized() const
    {
        return bInitialized;
    }

private:
    /** True if the resource has been initialized. */
    bool bInitialized = false;
};
