#pragma once
#include "PrimitiveComponent.h"
#include "Math/Color.h"

struct FLinearColor;

/**
 * 맵의 높이에 반비례하여 밀도를 생성
 * 2개의 포그 컬러 제공
 * 1. 도미넌트 디렉셔널 라이트 방향(없는 경우 수직 위) 반구
 * 2. 반대편 반구
 */
class UExponentialHeightFogComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UExponentialHeightFogComponent, UPrimitiveComponent)

    UExponentialHeightFogComponent();
    
public:
    /**
     * 안개 밀도
     */
    float FogDensity;

    /**
     * 높이 감소에 따라 밀도가 증가하는 방식을 조절할 수 있는 높이 밀도 인수
     * 값이 작을수록 전환 폭이 커짐
     */
    float FogHeightFalloff;

    /**
     * 카메라와 안개 시작 지점 간의 거리
     */
    float StartDistance;

    /**
     * 해당 거리를 지나는 씬 엘리먼트에는 안개가 적용되지 않음
     */
    float FogCutOffDistance;

    /**
     * 안개의 최대 불투명도
     * 1 : 불투명
     * 0 : 투명
     */
    float FogMaxOpacity;
    
    /**
     * 안개 내부의 스캐터링되는 색 (안개의 주요 색상)
     */
    FLinearColor FogInscatteringColor;

public:
    void SetFogDensity(float Density);

    void SetFogHeightFalloff(float HeightFalloff);

    void SetFogMaxOpacity(float MaxOpacity);

    void SetCutOffDistance(float CutOffDistance);
    
    void SetStartDistance(float InStartDistance);

    void SetFogInscatteringColor(FLinearColor Color);
};
