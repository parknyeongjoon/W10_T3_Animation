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

    // TBN 행렬 구성 요소 전달
    float3 normal = normalize(mul(input.normal, MInverseTranspose)); //0~1사이로 변환
    float3 tangent = normalize(mul(input.tangent, Model));
    float3 biTangent = normalize(cross(normal, tangent));

    // 재계산을 통해 orthogonal한 biTangent를 구함.
    // 만약 원래의 biTangent가 신뢰할 만하다면 normalize(input.biTangent)를 사용할 수도 있지만,
    // 교차곱을 통해 정상적인 수직 벡터를 얻는 것이 안전합니다.

    // 최종 TBN 행렬 구성 (T, B, N는 각각 한 열 또는 행이 될 수 있음, 아래 예제는 행 벡터로 구성)
    // row_major float4x4 TBN = float4x4(T, B, N, float4(0,0,0,1));
    float3x3 TBN = float3x3(tangent, biTangent, normal);

    output.TBN = TBN;
    output.normal = normal / 2 + 0.5;
    
    return output;
}