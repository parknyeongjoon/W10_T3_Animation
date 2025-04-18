#include "Color.h"

const FLinearColor FLinearColor::White(1.0f, 1.0f, 1.0f);
const FLinearColor FLinearColor::Black(0.0f, 0.0f, 0.0f);
const FLinearColor FLinearColor::Red(1.0f, 0.0f, 0.0f);
const FLinearColor FLinearColor::Green(0.0f, 1.0f, 0.0f);
const FLinearColor FLinearColor::Blue(0.0f, 0.0f, 1.0f);

FString FLinearColor::ToString() const
{
    return FString::Printf(TEXT("R=%3.3f G=%3.3f B=%3.3f A=%3.3f"), R, G, B, A);
}