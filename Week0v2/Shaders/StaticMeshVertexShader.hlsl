// MatrixBuffer: 변환 행렬 관리
cbuffer FMatrixConstants : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
    row_major float4x4 MInverseTranspose;
    bool isSelected;
    float3 padding; // 12바이트 패딩: 상수 버퍼 정렬을 위해 추가함
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float4 color : COLOR; // 전달할 색상
    float2 texcoord : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3x3 TBN: TEXCOORD2;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
       
    float4 worldPos = mul(input.position, Model);
    output.position = mul(worldPos, ViewProj);
    output.worldPos = worldPos.xyz;
    output.color = input.color;
    output.texcoord = input.texcoord;
    
    // 노멀 계산 (안전한 역전치 행렬 적용)
    float3 normal = mul(float4(input.normal, 0), MInverseTranspose);
    normal = normalize(normal);

    float3 tangent = normalize(mul(input.tangent, Model));

    // 탄젠트-노멀 직교화 (Gram-Schmidt 과정) 해야 안전함
    tangent = normalize(tangent - normal * dot(tangent, normal));

    // 바이탄젠트 계산 (안전한 교차곱)
    float3 biTangent = cross(normal, tangent);

    // 최종 TBN 행렬 구성 (T, B, N는 각각 한 열 또는 행이 될 수 있음, 아래 예제는 행 벡터로 구성)
    // row_major float4x4 TBN = float4x4(T, B, N, float4(0,0,0,1));
    float3x3 TBN = float3x3(tangent, biTangent, normal);

    output.TBN = TBN;
    output.normal = normal;
    
    return output;
}