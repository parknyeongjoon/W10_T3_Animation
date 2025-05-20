cbuffer FMatrixSeparatedMVPConstants : register(b0)
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
    float InstanceParticleRotation : INSTANCEROTATION;
    float InstanceParticleSubIndex : INSTANCESUBINDEX;
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
PS_INPUT mainVS(VS_INPUT input, uint instanceId : SV_InstanceID)
{
    PS_INPUT output;

    // 1. 카메라 기준 오른쪽, 위 방향 (View 행렬의 0,1행)
    float3 right = View[0].xyz;
    float3 up = View[1].xyz;

    // 2. 텍스처 좌표를 [-1,1] 쿼드 기준으로 변환
    float2 offset = (input.texcoord * 2.0f - 1.0f);

    // 3. 크기 적용하고 방향에 맞게 오프셋 생성
    float3 worldOffset = 
        right * offset.x * input.InstanceParticleSize.x * 0.5 +
        up * offset.y * input.InstanceParticleSize.y * 0.5;

    // 4. 최종 월드 위치
    float3 worldPos = input.InstanceParticleLocation + worldOffset;

    // 5. MVP 변환
    output.position = mul(float4(worldPos, 1.0f), Model);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Proj);

    output.color = input.color;
    return output;
}