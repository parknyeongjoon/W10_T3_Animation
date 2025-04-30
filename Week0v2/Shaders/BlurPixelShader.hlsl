Texture2D BlurTexture : register(t0);        // 핑퐁 버퍼의 최종 결과
SamplerState PointSampler : register(s0);     // 포인트 필터링 샘플러

cbuffer BlurConstants : register(b0) // 레지스터 슬롯은 엔진의 다른 버퍼와 겹치지 않게 선택 (b0는 예시)
{
    float TargetBlurStrength;  // 블러 강도
    float BlurRadius;          // 블러 반경
    float NDCX;                // 정규화된 텍셀 크기 X (1.0/화면 너비)
    float NDCY;                // 정규화된 텍셀 크기 Y (1.0/화면 높이)
}

struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

float4 mainPS(VS_OUT input) : SV_TARGET
{
    // 블러 세기 결정 (0에서 1 사이의 값)
    float blurStrength = saturate(TargetBlurStrength);
    
    // 블러 세기가 0이면 원본 이미지 반환
    if (blurStrength <= 0.001f)
    {
        return BlurTexture.Sample(PointSampler, input.uv);
    }
    
    // 블러를 적용할 가우시안 반경과 시그마
    float radius = max(1.0, BlurRadius);
    float sigma = radius / 2.0;
    
    // 최종 색상 초기화
    float4 finalColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0;
    
    // 수평 및 수직 블러를 동시에 적용 (단일 패스)
    // 최적화를 위해 고정된 샘플 수 사용
    const int MAX_RADIUS = 5;
    
    // 중앙 픽셀 먼저 샘플링
    float weight = 1.0;
    float4 centerColor = BlurTexture.Sample(PointSampler, input.uv);
    finalColor += centerColor * weight;
    totalWeight += weight;
    
    // 가우시안 블러 샘플링
    for (int x = 1; x <= MAX_RADIUS; x++)
    {
        // 수평 방향 가중치
        weight = exp(-(x*x) / (2.0f * sigma * sigma));
        
        // 왼쪽 샘플
        float2 offset = float2(-x * NDCX, 0);
        float4 sampleColor = BlurTexture.Sample(PointSampler, input.uv + offset);
        finalColor += sampleColor * weight;
        
        // 오른쪽 샘플
        offset = float2(x * NDCX, 0);
        sampleColor = BlurTexture.Sample(PointSampler, input.uv + offset);
        finalColor += sampleColor * weight;
        
        // 가중치 두 번 추가 (왼쪽/오른쪽)
        totalWeight += weight * 2.0;
    }
    
    for (int y = 1; y <= MAX_RADIUS; y++)
    {
        // 수직 방향 가중치
        weight = exp(-(y*y) / (2.0f * sigma * sigma));
        
        // 위쪽 샘플
        float2 offset = float2(0, -y * NDCY);
        float4 sampleColor = BlurTexture.Sample(PointSampler, input.uv + offset);
        finalColor += sampleColor * weight;
        
        // 아래쪽 샘플
        offset = float2(0, y * NDCY);
        sampleColor = BlurTexture.Sample(PointSampler, input.uv + offset);
        finalColor += sampleColor * weight;
        
        // 가중치 두 번 추가 (위/아래)
        totalWeight += weight * 2.0;
    }
    
    // 가중치 합으로 정규화
    finalColor /= totalWeight;
    
    // 블러 강도에 따라 원본 이미지와 블러된 이미지 보간
    float4 result = lerp(centerColor, finalColor, blurStrength);
    
    return float4(result.rgb, 1.0);
}