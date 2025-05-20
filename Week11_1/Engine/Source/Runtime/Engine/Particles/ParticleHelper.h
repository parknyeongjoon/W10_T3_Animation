#pragma once
#include "Math/Vector.h"
#include "Math/Color.h"
#include "Math/Vector4.h"

class UMaterial;
struct FParticleRequiredModule;

/**
 * Per-particle data sent to the GPU.
 */
struct FParticleSpriteVertex
{
    /** The position of the particle. */
    FVector Position;
    /** The size of the particle. */
    FVector2D Size;
    /** The rotation of the particle. */
    float Rotation;
    /** The sub-image index for the particle. */
    float SubImageIndex;
    
    // 아래부턴 GPU에서 필요한거
    /** The relative time of the particle. */
    // float RelativeTime;
    /** The previous position of the particle. */
    // FVector	OldPosition;
    /** Value that remains constant over the lifetime of a particle. */
    // float ParticleId;
    /** The color of the particle. */
    // FLinearColor Color;
};

// Per-particle data sent to the GPU.
struct FMeshParticleInstanceVertex
{
    /** The color of the particle. */
    FLinearColor Color;

    /** The instance to world transform of the particle. Translation vector is packed into W components. */
    FVector4 Transform[3];

    /** The velocity of the particle, XYZ: direction, W: speed. */
    FVector4 Velocity;

    /** The sub-image texture offsets for the particle. */
    int16 SubUVParams[4];

    /** The sub-image lerp value for the particle. */
    float SubUVLerp;

    /** The relative time of the particle. */
    float RelativeTime;
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
    // 48 bytes
    // FVector		    OldLocation;			// Last frame's location, used for collision
    FVector		    Location;				// Current location

    // 16 bytes
    // FVector		    BaseVelocity;			// Velocity = BaseVelocity at the start of each frame.
    float			Rotation;				// Rotation of particle (in Radians)

    // 16 bytes
    FVector		    Velocity;				// Current velocity, gets reset to BaseVelocity each frame to allow 
    float			BaseRotationRate;		// Initial angular velocity of particle (in Radians per second)

    // 16 bytes
    FVector		    BaseSize;				// Size = BaseSize at the start of each frame
    float			RotationRate;			// Current rotation rate, gets reset to BaseRotationRate each frame

    // 16 bytes
    FVector		    Size;					// Current size, gets reset to BaseSize each frame
    // int32			Flags;					// Flags indicating various particle states

    // 16 bytes
    // FLinearColor	Color;					// Current color of particle.

    // 16 bytes
    // FLinearColor	BaseColor;				// Base color of the particle

    // 16 bytes
    float			RelativeTime;			// Relative time, range is 0 (==spawn) to 1 (==death)
    float           Lifetime;	            // Reciprocal of lifetime
    // float			Placeholder0;
    // float			Placeholder1;
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

    FVector Scale;

    int32 SortMode;
};

struct FDynamicSpriteEmitterReplayDataBase : public FDynamicEmitterReplayDataBase
{
    UMaterial* MaterialInterface;
    struct FParticleRequiredModule* RequiredModule;
    int32							SubUVDataOffset;
    int32							OrbitModuleOffset;
    int32							DynamicParameterDataOffset;
    int32							CameraPayloadOffset;
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

struct FDynamicEmitterDataBase
{
    int32 EmitterIndex;
    
    virtual const FDynamicEmitterReplayDataBase& GetSource() const = 0;

    virtual int32 const GetDynamicVertexStride() const = 0;

    // Particle을 생성하지 않는 Emitter가 있을 수 있다면 가상함수 말고 아무행동 안하는 함수로 변경
    virtual void GetDynamicMeshElementsEmitter() const {};

};

struct FDynamicSpriteEmitterDataBase : public FDynamicEmitterDataBase
{

    FDynamicSpriteEmitterReplayDataBase Source;
    
    /** Returns the source data for this particle system */
    virtual const FDynamicSpriteEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }
    
    /**
 *	Sort the given sprite particles
 *
 *	@param	SorceMode			The sort mode to utilize (EParticleSortMode)
 *	@param	bLocalSpace			true if the emitter is using local space
 *	@param	ParticleCount		The number of particles
 *	@param	ParticleData		The actual particle data
 *	@param	ParticleStride		The stride between entries in the ParticleData array
 *	@param	ParticleIndices		Indirect index list into ParticleData
 *	@param	View				The scene view being rendered
 *	@param	LocalToWorld		The local to world transform of the component rendering the emitter
 *	@param	ParticleOrder		The array to fill in with ordered indices
 */
    void SortSpriteParticles();
    
    
    FDynamicEmitterReplayDataBase& GetSource();

    // (int32 SortMode, bool bLocalSpace, 
    //     int32 ParticleCount, const uint8* ParticleData, int32 ParticleStride, const uint16* ParticleIndices,
    //     const FSceneView* View, const FMatrix& LocalToWorld, FParticleOrder* ParticleOrder) const;
    virtual int32 const GetDynamicVertexStride() const = 0;
    // virtual int32 GetDynamicParameterVertexStride() const = 0;
};

struct FDynamicSpriteEmitterData : public FDynamicSpriteEmitterDataBase
{
    FDynamicSpriteEmitterReplayData Source;

	/** Returns the source data for this particle system */
	virtual const FDynamicSpriteEmitterReplayData& GetSource() const override
	{
		return Source;
	}
    
    virtual int32 const GetDynamicVertexStride() const override
    {
        return sizeof(FParticleSpriteVertex);
    }
};

struct FDynamicMeshEmitterData : public FDynamicSpriteEmitterData
{
    // virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    // {
    //     return Source;
    // }
    
    // virtual int32 GetDynamicVertexStride() const override
    // {
    //     return sizeof(FMeshParticleInstanceVertex);
    // }
    
    // virtual int32 GetDynamicParameterVertexStride() const override 
    // {
    //     return sizeof(FMeshParticleInstanceVertexDynamicParameter);
    // }

    // virtual void GetDynamicMeshElementsEmitter() const override;

};

FORCEINLINE FVector2D GetParticleSizeWithUVFlipInSign(const FBaseParticle& Particle, const FVector2D& ScaledSize)
{
    return FVector2D(
        Particle.BaseSize.X >= 0.0f ? ScaledSize.X : -ScaledSize.X,
        Particle.BaseSize.Y >= 0.0f ? ScaledSize.Y : -ScaledSize.Y);
}

#define DECLARE_PARTICLE(Name,Address)		\
	FBaseParticle& Name = *((FBaseParticle*) (Address));

#define DECLARE_PARTICLE_CONST(Name,Address)		\
	const FBaseParticle& Name = *((const FBaseParticle*) (Address));

#define DECLARE_PARTICLE_PTR(Name,Address)		\
	FBaseParticle* Name = (FBaseParticle*) (Address);