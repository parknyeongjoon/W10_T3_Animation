#pragma once
#include "Math/Vector.h"
#include "Math/Color.h"
#include "Math/Matrix.h"
#include "Math/Vector4.h"

class UMaterial;
class UStaticMesh;
struct FParticleRequiredModule;
class UTexture;
/**
 * Per-particle data sent to the GPU.
 */
struct FParticleSpriteVertex
{
    /** The position of the particle. */
    FVector Position;
    /** The relative time of the particle. */
    // float RelativeTime;
    /** The previous position of the particle. */
    // FVector	OldPosition;
    /** Value that remains constant over the lifetime of a particle. */
    // float ParticleId;
    /** The size of the particle. */
    FVector2D Size;
    /** The rotation of the particle. */
    float Rotation;
    /** The sub-image index for the particle. */
    float SubImageIndex;
    /** The color of the particle. */
    FLinearColor Color;
};

// Per-particle data sent to the GPU.
struct FMeshParticleInstanceVertex
{
    /** The color of the particle. */
    FMatrix Transform;

    FLinearColor Color;
    /** The instance to world transform of the particle. Translation vector is packed into W components. */
    //FVector4 Transform[3];
};

struct FMeshParticleInstanceVertexDynamicParameter
{
    /** The dynamic parameter of the particle. */
    float DynamicValue[4];
};

enum EDynamicEmitterType
{
    DET_Unknown = 0,
    DET_Sprite,
    DET_Mesh,
    DET_Beam2,
    DET_Ribbon,
    DET_AnimTrail,
    DET_Custom
};

/*-----------------------------------------------------------------------------
    FBaseParticle
-----------------------------------------------------------------------------*/
struct FBaseParticle
{
    FVector    Location;
    FVector    Velocity;
    float      RelativeTime;
    float      Lifetime;
    FVector    BaseVelocity;
    FVector    Rotation;
    float      RotationRate;
    FVector    Size;
    FColor     Color;
};

/*-----------------------------------------------------------------------------
    FParticleDataContainer
-----------------------------------------------------------------------------*/
struct FParticleDataContainer
{
    int32 MemBlockSize;
    int32 ParticleDataNumBytes;
    int32 ParticleIndicesNumShorts;
    uint8* ParticleData; // this is also the memory block we allocated
    uint16* ParticleIndices; // not allocated, this is at the end of the memory block

    FParticleDataContainer()
    : MemBlockSize(0)
    , ParticleDataNumBytes(0)
    , ParticleIndicesNumShorts(0)
    , ParticleData(nullptr)
    , ParticleIndices(nullptr)
    {
    }
    ~FParticleDataContainer()
    {
        Free();
    }
    void Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts);
    void Free();
};

/*-----------------------------------------------------------------------------
    FDynamicEmitterReplayDataBase
-----------------------------------------------------------------------------*/
struct FDynamicEmitterReplayDataBase
{
    /** The type of emitter. */
    EDynamicEmitterType eEmitterType;

    /** The number of particles currently active in this emitter. */
    int32 ActiveParticleCount;

    int32 ParticleStride;
    FParticleDataContainer DataContainer;

    FVector Scale = FVector::OneVector; // 1로 사용

    int32 SortMode = 0; // 사용중?
};

// 생성하고 eEmitterType 변경해주기
struct FDynamicSpriteEmitterReplayDataBase : public FDynamicEmitterReplayDataBase
{
    // TODO: UMaterial로 변경.
    //UMaterial* MaterialInterface;
    UTexture* Texture;
    //struct UParticleModuleRequired* RequiredModule;
    FVector EmitterOrigin = FVector::ZeroVector;
    
    int32							SubUVDataOffset;
    int32							OrbitModuleOffset;
    int32							DynamicParameterDataOffset;
    int32							CameraPayloadOffset;
    int32							SubImages_Horizontal;
    int32							SubImages_Vertical;
};

// 생성하고 eEmitterType 변경해주기
struct FDynamicMeshEmitterReplayData : public FDynamicSpriteEmitterReplayDataBase
{
};

struct FFullSubUVPayload
{
    // The integer portion indicates the sub-image index.
    // The fractional portion indicates the lerp factor.
    float ImageIndex;
    float RandomImageTime;
};

/** Source data for Sprite emitters */
struct FDynamicSpriteEmitterReplayData
    : public FDynamicSpriteEmitterReplayDataBase
{
    /** Constructor */
    FDynamicSpriteEmitterReplayData()
    {
    }
};

// !!! 생성하지 말것
struct FDynamicEmitterDataBase
{
    int32 EmitterIndex; // 현재 UParticleSystemComponent::UpdateDynamicData()에서 처리
    
    virtual const FDynamicEmitterReplayDataBase& GetSource() const = 0;

    virtual int32 const GetDynamicVertexStride() const = 0;

    // Particle을 생성하지 않는 Emitter가 있을 수 있다면 가상함수 말고 아무행동 안하는 함수로 변경
    virtual void GetDynamicMeshElementsEmitter() const {};

    // 여기 머터리얼

};

// !!! 생성하지말것
struct FDynamicSpriteEmitterDataBase : public FDynamicEmitterDataBase
{    
    void SortSpriteParticles();
    
    //FDynamicEmitterReplayDataBase& GetSource();

    // (int32 SortMode, bool bLocalSpace, 
    //     int32 ParticleCount, const uint8* ParticleData, int32 ParticleStride, const uint16* ParticleIndices,
    //     const FSceneView* View, const FMatrix& LocalToWorld, FParticleOrder* ParticleOrder) const;
    virtual int32 const GetDynamicVertexStride() const = 0;
    // virtual int32 GetDynamicParameterVertexStride() const = 0;
};

/*
실제 new 할때 사용해야하는 구조체
*/
struct FDynamicSpriteEmitterData : public FDynamicSpriteEmitterDataBase
{
    FDynamicSpriteEmitterReplayData Source;

    /** Returns the source data for this particle system */
    virtual const FDynamicSpriteEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }

    virtual int32 const GetDynamicVertexStride() const override
    {
        return sizeof(FParticleSpriteVertex);
    }
};

/*
실제 new 할때 사용해야하는 구조체
*/
struct FDynamicMeshEmitterData : public FDynamicSpriteEmitterDataBase
{
    FDynamicMeshEmitterReplayData Source;

    /** Returns the source data for this particle system */
    virtual const FDynamicSpriteEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }

    UStaticMesh* Mesh;
};

FORCEINLINE FVector2D GetParticleSizeWithUVFlipInSign(const FBaseParticle& Particle, const FVector2D& ScaledSize)
{
    return FVector2D(
        Particle.Size.X >= 0.0f ? ScaledSize.X : -ScaledSize.X,
        Particle.Size.Y >= 0.0f ? ScaledSize.Y : -ScaledSize.Y);
}