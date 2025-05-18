
#include "PropertyEditor/PreviewControlEditorPanel.h"
#include "PropertyEditor/PreviewControlEditorPanel.h"

cbuffer FMatrixBuffer : register(b0)
{
    row_major float4x4 Model; //Emitter 위치
    row_major float4x4 View;
    row_major float4x4 Proj;
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;

    float3 InstanceParticleLocation : INSTANCEPOS;
    float2 InstanceParticleSize : INSTANCESIZE;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float4 color : COLOR; // 전달할 색상
    float2 texcoord : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3x3 TBN: TEXCOORD3;
};

/////////////////////////////////////////////////////////////////////////
// 메인 버텍스 셰이더
/////////////////////////////////////////////////////////////////////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    //카메라 방향 기준으로 쿼드 offset계산
    float3 right = View[0].xyz;
    float3 up = View[1].xyz;

    float2 offset = input.texcoord / 2 - 1; //
    float3 worldOffset = right * offset.x * input.InstanceParticleSize.x * 0.5+
                            up * offset.y * input.InstanceParticleSize.y * 0.5;

    float3 worldPos = mul(input.InstanceParticleLocation, worldOffset);
    
    // 출력 변환
    output.position = mul(float4(input.position), Model);
    output.position = mul(output.position, ViewProj);
    output.color = input.color;
    return output;
}

/* 빌보드 M구하는 방식
FMatrix CameraView = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix();

    CameraView.M[0][3] = 0.0f;
    CameraView.M[1][3] = 0.0f;
    CameraView.M[2][3] = 0.0f;


    CameraView.M[3][0] = 0.0f;
    CameraView.M[3][1] = 0.0f;
    CameraView.M[3][2] = 0.0f;
    CameraView.M[3][3] = 1.0f;


    CameraView.M[0][2] = -CameraView.M[0][2];
    CameraView.M[1][2] = -CameraView.M[1][2];
    CameraView.M[2][2] = -CameraView.M[2][2];
    FMatrix LookAtCamera = FMatrix::Transpose(CameraView);
    
    FVector worldLocation = GetWorldLocation();
    FVector worldScale = RelativeScale;
    FMatrix S = FMatrix::CreateScaleMatrix(worldScale.X, worldScale.Y, worldScale.Z);
    FMatrix R = LookAtCamera;
    FMatrix T = FMatrix::CreateTranslationMatrix(worldLocation);
 **/