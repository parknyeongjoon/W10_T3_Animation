// C++ 에서 전달받을 레터박스 관련 상수 버퍼
cbuffer FLetterBoxConstants : register(b1) // 레지스터 슬롯은 다른 버퍼와 겹치지 않게 선택 (b0가 Fade에 사용되었다면 b1 사용)
{
    // 레터박스 바의 색상 (보통 검은색 float4(0,0,0,1))
    float4 LetterboxColor;
    // 레터박스 바의 크기 (화면 높이 기준, 0.0 ~ 0.5 범위)
    // 예를 들어 0.1이면 상단 10%, 하단 10%가 바 영역이 됨.
    float  LetterboxSize;
    float3 Padding;
    
};

Texture2D SceneTexture : register(t1);
SamplerState SamplerLinear : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION; // 화면 좌표
    float2 uv : TEXCOORD0;         // UV 좌표 (0,0 좌상단 ~ 1,1 우하단)
};

float4 mainPS(VS_OUT input) : SV_TARGET
{
    // 현재 픽셀의 세로 위치(V 좌표)를 확인
    // input.uv.y 값은 화면 상단이 0.0, 하단이 1.0 입니다.

    // 1. 상단 바 영역인지 확인
    bool isTopBar = input.uv.y < LetterboxSize;

    // 2. 하단 바 영역인지 확인
    bool isBottomBar = input.uv.y > (1.0f - LetterboxSize);

    // 3. 상단 또는 하단 바 영역에 해당하면 레터박스 색상 출력
    if (isTopBar || isBottomBar)
    {
        return LetterboxColor;
    }
    else // 4. 바 영역이 아니면 (즉, 보이는 영역이면) 원본 씬 색상 출력
    {
        float4 sceneColor = SceneTexture.Sample(SamplerLinear, input.uv);
        return sceneColor;
    }
}