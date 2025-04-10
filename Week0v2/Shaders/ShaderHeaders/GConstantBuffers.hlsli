#pragma once

////////
/// 공용: 13 ~ 15
///////
cbuffer ObjectBuffer : register(b11)
{
    row_major matrix ModelMatrix;
    row_major matrix InverseTranspose;
    float4 UUID;
    bool IsSelected;
    float3 ObjectPadding;
};

cbuffer ViewBuffer : register(b12)
{
    row_major matrix ViewMatrix;
    row_major matrix InvViewMatrix;
    float3 ViewLocation;
    float ViewPadding;
}

cbuffer ProjectionBuffer : register(b13)
{
    row_major matrix ProjectionMatrix;
    row_major matrix InvProjectionMatrix;
    float NearClip;
    float FarClip;
    float2 ProjectionPadding;
}