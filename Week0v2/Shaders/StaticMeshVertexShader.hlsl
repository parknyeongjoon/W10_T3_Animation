// MatrixBuffer: 변환 행렬 관리
cbuffer FMatrixConstants : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
    row_major float4x4 MInverseTranspose;
    bool isSelected;
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangentNormal : TANGENT;
    float2 texcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float4 color : COLOR; // 전달할 색상
    float2 texcoord : TEXCOORD0;
    float3 tangent: TEXCOORD1;
    float3 biTangent: TEXCOORD2;
    float3 normal: TEXCOORD3;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
       
    float4 worldPos = mul(input.position, Model);
    output.position = mul(worldPos, ViewProj);
    output.worldPos = worldPos.xyz;
    output.color = input.color;
    output.texcoord = input.texcoord;

    // TBN 행렬 구성 요소 전달
    output.normal = normalize(mul(input.normal, MInverseTranspose));
    output.tangent = normalize(mul(input.tangentNormal, Model));
    output.biTangent = normalize(cross(output.normal, output.tangent));
    
    return output;
}