#include "Color.h"

const FColor FColor::White(255, 255, 255, 255);
const FColor FColor::Black(0, 0, 0, 255);
const FColor FColor::Red(255, 0, 0, 255);
const FColor FColor::Green(0, 255, 0, 255);
const FColor FColor::Blue(0, 0, 255, 255);
const FColor FColor::Yellow(255, 255, 0, 255);
const FColor FColor::Transparent(0, 0, 0, 0);
const FColor FColor::Cyan(0, 255, 255, 255);
const FColor FColor::Magenta(255, 0, 255, 255);
const FColor FColor::Gray(128, 128, 128, 255);
const FColor FColor::Orange(255, 165, 0, 255);
const FColor FColor::Purple(128, 0, 128, 255);
const FColor FColor::Silver(192, 192, 192, 255);
const FColor FColor::Emerald(0, 201, 87, 255);

const FLinearColor FLinearColor::White(1.0f, 1.0f, 1.0f);
const FLinearColor FLinearColor::Black(0.0f, 0.0f, 0.0f);
const FLinearColor FLinearColor::Red(1.0f, 0.0f, 0.0f);
const FLinearColor FLinearColor::Green(0.0f, 1.0f, 0.0f);
const FLinearColor FLinearColor::Blue(0.0f, 0.0f, 1.0f);

float FLinearColor::LinearToSRGB(float InC)
{
    if (InC <= 0.0031308f)
    {
        return InC * 12.92f;
    }
    else
    {
        return 1.055f * FMath::Pow(InC, 0.41666667f) - 0.055f;
    }
}

FColor FLinearColor::ToColorSRGB() const
{
    return FColor(
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(LinearToSRGB(R) * 255.0f + 0.5f), 0, 255)),
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(LinearToSRGB(G) * 255.0f + 0.5f), 0, 255)),
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(LinearToSRGB(B) * 255.0f + 0.5f), 0, 255)),
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(A * 255.0f + 0.5f), 0, 255))
    );
}

FColor FLinearColor::ToColorRawRGB8() const
{
    return FColor(
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(R * 255.0f + 0.5f), 0, 255)),
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(G * 255.0f + 0.5f), 0, 255)),
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(B * 255.0f + 0.5f), 0, 255)),
        static_cast<uint8>(FMath::Clamp(static_cast<int32>(A * 255.0f + 0.5f), 0, 255))
    );
}

FString FLinearColor::ToString() const
{
    return FString::Printf(TEXT("R=%3.3f G=%3.3f B=%3.3f A=%3.3f"), R, G, B, A);
}