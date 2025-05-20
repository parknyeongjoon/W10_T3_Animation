#pragma once

#define DECLARE_PARTICLE(Name,Address)		\
	FBaseParticle& Name = *((FBaseParticle*) (Address));

#define DECLARE_PARTICLE_CONST(Name,Address)		\
	const FBaseParticle& Name = *((const FBaseParticle*) (Address));

#define DECLARE_PARTICLE_PTR(Name,Address)		\
	FBaseParticle* Name = (FBaseParticle*) (Address);

#define BEGIN_UPDATE_LOOP \
    uint8* ParticleData = Owner->ParticleData; \
    uint16* ParticleIndices = Owner->ParticleIndices; \
    int32 ParticleStride = Owner->ParticleStride; \
    int32 ActiveParticles = Owner->ActiveParticles; \
    for(int32 i = 0; i<ActiveParticles; i++) \
    {\
        const int32 Index = ParticleIndices[i]; \
        uint8* Address = ParticleData + Index * ParticleStride; \
        FBaseParticle& Particle = *((FBaseParticle*)Address);

#define END_UPDATE_LOOP \
    }

#define SPAWN_INIT \
    const int32 ActiveCount = Owner->ActiveParticles; \
    const int32 ParticleStride = Owner->ParticleStride; \
    uint8* ParticleData = Owner->ParticleData;

