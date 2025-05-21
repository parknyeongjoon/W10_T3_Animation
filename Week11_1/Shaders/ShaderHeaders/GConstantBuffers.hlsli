#pragma once

////////
/// 공용: 13 ~ 15
///////
cbuffer FObjectConstant : register(b11)
{
    row_major matrix ModelMatrix;
    row_major matrix Inverse ;
    bool IsSelected;
    float3 ObjectPadding;
};

cbuffer FViewportInfo : register(b12)
{
    float2 ViewportSize;
    float2 ViewportOffset;
}

cbuffer FProjectionConstant : register(b13)
{
    row_major matrix ProjectionMatrix;
    row_major matrix InvProjectionMatrix;
    float NearClip;
    float FarClip;
    float2 ProjectionPadding;
}