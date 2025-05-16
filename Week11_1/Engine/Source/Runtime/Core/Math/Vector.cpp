#include "Vector.h"

const FVector2D FVector2D::ZeroVector = FVector2D(0, 0);
const FVector2D FVector2D::OneVector = FVector2D(1, 1);

const FVector FVector::ZeroVector = {0.0f, 0.0f, 0.0f};
const FVector FVector::OneVector = {1.0f, 1.0f, 1.0f};
const FVector FVector::ForwardVector = {1.0f, 0.0f, 0.0f};
const FVector FVector::RightVector = {0.0f, 1.0f, 0.0f};
const FVector FVector::UpVector = {0.0f, 0.0f, 1.0f};

const FVector FVector::XAxisVector = FVector(1, 0, 0);
const FVector FVector::YAxisVector = FVector(0, 1, 0);
const FVector FVector::ZAxisVector = FVector(0, 0, 1);