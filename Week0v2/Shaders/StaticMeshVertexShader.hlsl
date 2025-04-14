// MatrixBuffer: 변환 행렬 관리
cbuffer FMatrixConstants : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
    row_major float4x4 MInverse;
    bool isSelected;
    float3 padding; // 12바이트 패딩: 상수 버퍼 정렬을 위해 추가함
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float2 texcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float4 color : COLOR; // 전달할 색상
    float3 normal : NORMAL; // 정규화된 노멀 벡터
    bool normalFlag : TEXCOORD0; // 노멀 유효성 플래그 (1.0: 유효, 0.0: 무효)
    float2 texcoord : TEXCOORD1;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
       
    float4 worldPos = mul(input.position, Model);
    output.position = mul(worldPos, ViewProj);
    // 위치 변환
    output.worldPos = worldPos.xyz;
    output.color = input.color;

    // 입력 normal 값의 길이 확인
    float normalThreshold = 0.001;
    float normalLen = length(input.normal);
    
    if (normalLen < normalThreshold)
    {
        output.normalFlag = 0.0;
    }
    else
    {
        //output.normal = normalize(input.normal);
        output.normal = mul(input.normal, MInverse);
        output.normalFlag = 1.0;
    }
    output.texcoord = input.texcoord;
    
    return output;
}