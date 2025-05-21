#include "ParticleModuleDefaults.h"
#include "Math/Vector.h"

const FWString ParticleModuleDefaults::Required::TexturePath = L"Contents/Textures/Particles/hit_2.png";
//L"Contents/Textures/Particles/jewel_particle_yellow@2x.png";
const FVector ParticleModuleDefaults::Required::EmitterOrigin = FVector::ZeroVector;
const FRotator ParticleModuleDefaults::Required::EmitterRotation = FRotator::ZeroRotator;

const FVector ParticleModuleDefaults::Velocity::LinearVelocity = FVector(0.f, 0.f, 10.f);

const FVector ParticleModuleDefaults::Color::MinColor = FVector(0.0f, 0.0f, 0.0f);
const FVector ParticleModuleDefaults::Color::MaxColor = FVector(1.0f, 1.0f, 1.0f);

const FVector ParticleModuleDefaults::Location::Min = FVector(-10, -10, -10);
const FVector ParticleModuleDefaults::Location::Max = FVector(10, 10, 10);

const FVector ParticleModuleDefaults::Size::Min = FVector(0.8, 0.8, 0.8);
const FVector ParticleModuleDefaults::Size::Max = FVector(1.2, 1.2, 1.2);