#pragma once
#include "UnrealClient.h"
//#include "UnrealEd/EditorViewportClient.h"

FViewport::FViewport()
{
}



FViewport::~FViewport()
{
}

void FViewport::Initialize()
{
}

void FViewport::ResizeViewport(float InWidth, float InHeight)
{
    float width = InWidth;
    float height = InHeight;
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    switch (viewLocation)
    {
    case EViewScreenLocation::TopLeft:
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = halfWidth;
        viewport.Height = halfHeight;
        break;
    case EViewScreenLocation::TopRight:
        viewport.TopLeftX = halfWidth;
        viewport.TopLeftY = 0.0f;
        viewport.Width = halfWidth;
        viewport.Height = halfHeight;
        break;
    case EViewScreenLocation::BottomLeft:
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = halfHeight;
        viewport.Width = halfWidth;
        viewport.Height = halfHeight;
        break;
    case EViewScreenLocation::BottomRight:
        viewport.TopLeftX = halfWidth;
        viewport.TopLeftY = halfHeight;
        viewport.Width = halfWidth;
        viewport.Height = halfHeight;
        break;
    default:
        break;
    }
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
}

void FViewport::ResizeViewport(FRect Top, FRect Bottom, FRect Left, FRect Right)
{
    switch (viewLocation)
    {
    case EViewScreenLocation::TopLeft:
        viewport.TopLeftX = Left.leftTopX;
        viewport.TopLeftY = Top.leftTopY;
        viewport.Width = Left.width;
        viewport.Height = Top.height;
        break;
    case EViewScreenLocation::TopRight:
        viewport.TopLeftX = Right.leftTopX;
        viewport.TopLeftY = Top.leftTopY;
        viewport.Width = Right.width;
        viewport.Height = Top.height;
        break;
    case EViewScreenLocation::BottomLeft:
        viewport.TopLeftX = Left.leftTopX;
        viewport.TopLeftY = Bottom.leftTopY;
        viewport.Width = Left.width;
        viewport.Height = Bottom.height;
        break;
    case EViewScreenLocation::BottomRight:
        viewport.TopLeftX = Right.leftTopX;
        viewport.TopLeftY = Bottom.leftTopY;
        viewport.Width = Right.width;
        viewport.Height = Bottom.height;
        break;
    default:
        break;
    }
}

void FViewport::ResizeViewport(FRect newRect)
{
    viewport.TopLeftX = newRect.leftTopX;
    viewport.TopLeftY = newRect.leftTopY;
    viewport.Width = newRect.width;
    viewport.Height = newRect.height;
}

