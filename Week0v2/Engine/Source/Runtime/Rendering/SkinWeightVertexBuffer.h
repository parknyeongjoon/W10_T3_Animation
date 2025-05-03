#pragma once
#include "Container/Array.h"

typedef uint16 FBoneIndexType;

/** Max number of bone influences that a single skinned vert can have. */
#define MAX_TOTAL_INFLUENCES 4

/** An runtime structure for passing data to FSkinWeightVertexBuffer */
struct FSkinWeightInfo
{
    FBoneIndexType	InfluenceBones[MAX_TOTAL_INFLUENCES];
    uint16			InfluenceWeights[MAX_TOTAL_INFLUENCES];
};

class FSkinWeightVertexBuffer
{
public:
    /** Default constructor. */
    FSkinWeightVertexBuffer();

    /** Constructor (copy) */
    FSkinWeightVertexBuffer(const FSkinWeightVertexBuffer& Other);

    /** Destructor. */
    ~FSkinWeightVertexBuffer();

    /** Assignment. Assumes that vertex buffer will be rebuilt */
    FSkinWeightVertexBuffer& operator=(const FSkinWeightVertexBuffer& Other);

    /** Delete existing resources */
    void CleanUp();

    FSkinWeightVertexBuffer& operator=(const TArray<FSkinWeightInfo>& InWeights);

    void GetSkinWeights(TArray<FSkinWeightInfo>& OutVertices) const;
    FSkinWeightInfo GetVertexSkinWeights(uint32 VertexIndex) const;

    void CopySkinWeightRawDataFromBuffer(const uint8* InSkinWeightData, uint32 InNumVertices);


    FORCEINLINE uint32 GetNumVertices() const
    { return DataVertexBuffer.GetNumVertices(); }

    /** @return total size of data in resource array */
    FORCEINLINE uint32 GetVertexDataSize() const
    { return LookupVertexBuffer.GetVertexDataSize() + DataVertexBuffer.GetVertexDataSize(); }


    void SetNeedsCPUAccess(bool bInNeedsCPUAccess)
    {
        DataVertexBuffer.SetNeedsCPUAccess(bInNeedsCPUAccess);
        LookupVertexBuffer.SetNeedsCPUAccess(bInNeedsCPUAccess);
    }

    bool GetNeedsCPUAccess() const
    { return DataVertexBuffer.GetNeedsCPUAccess(); }

    void SetMaxBoneInfluences(uint32 InMaxBoneInfluences)
    { DataVertexBuffer.SetMaxBoneInfluences(InMaxBoneInfluences); }

    uint32 GetMaxBoneInfluences() const
    { return DataVertexBuffer.GetMaxBoneInfluences(); }

    void SetUse16BitBoneIndex(bool bInUse16BitBoneIndex)
    { DataVertexBuffer.SetUse16BitBoneIndex(bInUse16BitBoneIndex); }

    bool Use16BitBoneIndex() const
    { return DataVertexBuffer.Use16BitBoneIndex(); }

    void SetUse16BitBoneWeight(bool bInUse16BitBoneWeight)
    { DataVertexBuffer.SetUse16BitBoneWeight(bInUse16BitBoneWeight); }
	
    bool Use16BitBoneWeight() const
    { return DataVertexBuffer.Use16BitBoneWeight(); }
    
    uint32 GetBoneIndexByteSize() const
    { return DataVertexBuffer.GetBoneIndexByteSize(); }

    uint32 GetBoneWeightByteSize() const
    { return DataVertexBuffer.GetBoneWeightByteSize(); }
	
    uint32 GetBoneIndexAndWeightByteSize() const
    { return DataVertexBuffer.GetBoneIndexAndWeightByteSize(); }
	
    bool GetVariableBonesPerVertex() const
    { return DataVertexBuffer.GetVariableBonesPerVertex(); }

    uint32 GetConstantInfluencesVertexStride() const
    { return DataVertexBuffer.GetConstantInfluencesVertexStride(); }

    uint32 GetConstantInfluencesBoneWeightsOffset() const
    { return DataVertexBuffer.GetConstantInfluencesBoneWeightsOffset(); }

    FSkinWeightDataVertexBuffer* GetDataVertexBuffer()
    { return &DataVertexBuffer; }
	
    const FSkinWeightDataVertexBuffer* GetDataVertexBuffer() const
    { return &DataVertexBuffer; }

    const FSkinWeightLookupVertexBuffer* GetLookupVertexBuffer() const
    { return &LookupVertexBuffer; }
    
    ENGINE_API GPUSkinBoneInfluenceType GetBoneInfluenceType() const;
    ENGINE_API void GetVertexInfluenceOffsetCount(uint32 VertexIndex, uint32& VertexWeightOffset, uint32& VertexInfluenceCount) const;
    ENGINE_API bool GetRigidWeightBone(uint32 VertexIndex, int32& OutBoneIndex) const;
    ENGINE_API uint32 GetBoneIndex(uint32 VertexIndex, uint32 InfluenceIndex) const;
    ENGINE_API void SetBoneIndex(uint32 VertexIndex, uint32 InfluenceIndex, uint32 BoneIndex);
    ENGINE_API uint16 GetBoneWeight(uint32 VertexIndex, uint32 InfluenceIndex) const;
    ENGINE_API void SetBoneWeight(uint32 VertexIndex, uint32 InfluenceIndex, uint16 BoneWeight);
    ENGINE_API void ResetVertexBoneWeights(uint32 VertexIndex);

    
private:
    /** Skin weights for skinning */
    FSkinWeightDataVertexBuffer		DataVertexBuffer;

    /** Skin weights lookup buffer */
    FSkinWeightLookupVertexBuffer	LookupVertexBuffer;
};
