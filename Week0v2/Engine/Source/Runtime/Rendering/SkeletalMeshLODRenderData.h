#pragma once
#include "SkinWeightVertexBuffer.h"
#include "Container/Array.h"
#include "Container/String.h"

struct FSkelMeshRenderSection
{
	uint16 MaterialIndex;
    uint32 BaseIndex;
    uint32 NumTriangles;
    
    // bool bRecomputeTangent;
    // bool bCastShadow;
    // ESkinVertexColorChannel RecomputeTangentsVertexMaskChannel;

    /** The offset into the LOD's vertex buffer of this section's vertices. */
    uint32 BaseVertexIndex;

    TArray<FBoneIndexType> BoneMap;
    uint32 NumVertices;
    int32 MaxBoneInfluences;

    /** Index Buffer containting all duplicated vertices in the section and a buffer containing which indices into the index buffer are relevant per vertex **/
    // FDuplicatedVerticesBuffer DuplicatedVerticesBuffer;

    /** Disabled sections will not be collected when rendering, controlled from the source section in the skeletal mesh asset */
    bool bDisabled;
};

// 해당 Class는 UE의 FSkeletalMeshLODRenderData의 구성만 따라갑니다.
class FSkeletalMeshLODRenderData
{
public:    
    /** Info about each section of this LOD for rendering */
    TArray<FSkelMeshRenderSection> RenderSections;

    FString VertexBufferName;
    FString IndexBufferName;
    
    // // Index Buffer (MultiSize: 16bit or 32bit)
    // FMultiSizeIndexContainer	MultiSizeIndexContainer;
    //
    // /** static vertices from chunks for skinning on GPU */
    // FStaticMeshVertexBuffers	StaticVertexBuffers;

    /** Skin weights for skinning */
    //FSkinWeightVertexBuffer		SkinWeightVertexBuffer;

    /** Skin weight profile data structures, can contain multiple profiles and their runtime FSkinWeightVertexBuffer */
    // FSkinWeightProfilesData SkinWeightProfilesData;

    // FSkeletalMeshVertexAttributeRenderData VertexAttributeBuffers;

    // GPU에서 사용할 최소 본 리스트
    // TArray<FBoneIndexType> ActiveBoneIndices;

    // cpu에서 계산할 모든 본
    TArray<FBoneIndexType> RequiredBones;

    // RenderData 전체 사이즈
    // uint32 BuffersSize;

    /**
    * Initialize the LOD's render resources.
    *
    * @param Parent Parent mesh
    */
    // void InitResources(bool bNeedsVertexColors, int32 LODIndex, TArray<class UMorphTarget*>& InMorphTargets, USkinnedAsset* Owner);

    /**
    * Releases the LOD's render resources.
    */
    // void ReleaseResources();

    /**
    * Releases the LOD's CPU render resources.
    */
    // void ReleaseCPUResources(bool bForStreaming = false);

    /** Constructor (default) */
    FSkeletalMeshLODRenderData() = default;
    // FSkeletalMeshLODRenderData(bool bAddRef = true) : BuffersSize(0)
    // {
    //     if (bAddRef)
    //     {
    //         AddRef();
    //     }
    // }

    FORCEINLINE ~FSkeletalMeshLODRenderData()
    {
        // check(GetRefCount() == 0);
    }

    // Serialize

    // uint32 GetNumVertices() const
    // {
    //     return StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();
    // }

    // uint32 GetVertexBufferMaxBoneInfluences() const
    // {
    //     return SkinWeightVertexBuffer.GetMaxBoneInfluences();
    // }

    // bool DoesVertexBufferUse16BitBoneIndex() const
    // {
    //     return SkinWeightVertexBuffer.Use16BitBoneIndex();
    // }

    // uint32 GetNumTexCoords() const
    // {
    //     return StaticVertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords();
    // }

    /** Checks whether or not the skin weight buffer has been overridden 'by default' and if not return the original Skin Weight buffer */
    // FSkinWeightVertexBuffer* GetSkinWeightVertexBuffer() 
    // {
    //     FSkinWeightVertexBuffer* OverrideBuffer = SkinWeightProfilesData.GetDefaultOverrideBuffer();
    //     return OverrideBuffer != nullptr ? OverrideBuffer : &SkinWeightVertexBuffer;
    // }

    /** Checks whether or not the skin weight buffer has been overridden 'by default' and if not return the original Skin Weight buffer */
    // const FSkinWeightVertexBuffer* GetSkinWeightVertexBuffer() const
    // {
    //     FSkinWeightVertexBuffer* OverrideBuffer = SkinWeightProfilesData.GetDefaultOverrideBuffer();
    //     return OverrideBuffer != nullptr ? OverrideBuffer : &SkinWeightVertexBuffer;
    // }

    /** Utility function for returning total number of faces in this LOD. */
    // int32 GetTotalFaces() const;

    /** Utility for finding the section that a particular vertex is in. */
    // void GetSectionFromVertexIndex(int32 InVertIndex, int32& OutSectionIndex, int32& OutVertIndex) const;

    /**
    * Get Resource Size
    */
    // void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) const;

    /** Get the estimated memory overhead of buffers marked as NeedsCPUAccess. */
    // SIZE_T GetCPUAccessMemoryOverhead() const;

    // O(1)
    // @return -1 if not found
    // uint32 FindSectionIndex(const FSkelMeshRenderSection& Section) const;

    // friend class FSkeletalMeshRenderData;
};
