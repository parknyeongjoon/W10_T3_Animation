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

cbuffer FViewConstant : register(b12)
{
    row_major matrix ViewMatrix;
    row_major matrix InvViewMatrix;
    float3 ViewLocation;
    float ViewPadding;
}

cbuffer FProjectionConstant : register(b13)
{
    row_major matrix ProjectionMatrix;
    row_major matrix InvProjectionMatrix;
    float NearClip;
    float FarClip;
    float2 ProjectionPadding;
}