cbuffer FMatrixSeparatedMVPConstants : register(b0)
{
    row_major float4x4 Model; //Emitter 위치
    row_major float4x4 View;
    row_major float4x4 Proj;
    row_major float4x4 InvView;
};

cbuffer FTextureCountConstants : register(b1)
{
    int SubUVCountX;
    int SubUVCountY;
    float2 TextureRectPadding;
}

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
    float4 InstanceParticleColor : INSTANCECOLOR;
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

    // 1. 카메라의 월드 공간 Right 및 Up 벡터 추출
    // InvView는 카메라의 월드 변환 행렬. row_major이므로:
    float3 cameraRightWS = InvView[0].xyz;
    float3 cameraUpWS    = InvView[1].xyz;

    // 2. 쿼드 메쉬의 로컬 정점 위치 (-1 ~ +1 범위)
    float2 localPos = input.position.xy;

    // 3. 파티클 회전 적용 (카메라를 바라보는 평면에서의 2D 회전)
    float s = sin(input.InstanceParticleRotation);
    float c = cos(input.InstanceParticleRotation);

    // float2x2 rotationMatrix = float2x2(c, -s, s, c); // 2D 회전 행렬
                                                     // HLSL은 column-major로 행렬을 곱하므로 mul(vector, matrix) 순서가 일반적.
                                                     // 여기서는 직접 계산.
    float2 rotatedLocalPos;
    rotatedLocalPos.x = localPos.x * c - localPos.y * s;
    rotatedLocalPos.y = localPos.x * s + localPos.y * c;
    // 또는: rotatedLocalPos = mul(localPos, rotationMatrix); // 만약 rotationMatrix를 row-major 스타일로 정의했다면
    // 또는: rotatedLocalPos = mul(rotationMatrix, localPos); // 만약 rotationMatrix를 column-major 스타일로 정의했다면

    // 4. 월드 공간에서 파티클 중심으로부터의 정점 오프셋 계산
    // localPos는 -1에서 1이므로, 크기를 절반으로 나눠서 적용
    float3 offset = cameraRightWS * rotatedLocalPos.x * (input.InstanceParticleSize.x * 0.5f) +
                    cameraUpWS    * rotatedLocalPos.y * (input.InstanceParticleSize.y * 0.5f);

    // 5. 최종 정점의 월드 위치 계산
    // InstanceParticleLocation이 이미 파티클의 최종 월드 위치라고 가정
    float4 worldPosition = float4(input.InstanceParticleLocation + offset, 1.0f);

    // (선택 사항) 만약 InstanceParticleLocation이 이미터의 로컬 좌표라면,
    // Model 행렬(이미터의 월드 변환)을 적용해야 합니다.
    // float4 particleWorldCenter = mul(float4(input.InstanceParticleLocation, 1.0f), Model);
    // worldPosition = float4(particleWorldCenter.xyz + offset, 1.0f);

    // 6. View-Projection 변환
    // row_major 행렬이므로 mul(vector, matrix) 순서.
    output.position = mul(worldPosition, Model);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Proj);

    // // 7. 텍스처 좌표 및 기타 데이터 전달
    float2 cellSize = float2(1.0 / SubUVCountX, 1.0 / SubUVCountY);
    int frameX = input.InstanceParticleSubIndex % SubUVCountX;
    int frameY = input.InstanceParticleSubIndex / SubUVCountY;
    float2 uvOffset = float2(frameX, frameY) * cellSize;
    output.texcoord = input.texcoord * cellSize + uvOffset;
    //일단 input.color에 의미있는 값이 없어서 안곱해주는중 -> input.color로 안하고 InstanceColor쓰는 이유는 모든 파티클의 색깔을 다르게 하고싶어서
    output.color = input.InstanceParticleColor;// * input.color; 
    return output;
}