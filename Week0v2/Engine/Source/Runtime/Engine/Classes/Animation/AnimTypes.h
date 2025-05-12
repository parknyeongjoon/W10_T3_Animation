#pragma once
#include "Container/Array.h"
#include "UObject/NameTypes.h"
#include "Math/Vector.h"
#include "Math/Quat.h"

enum class EAnimInterpolationType : std::uint8_t
{
    None,
    Linear,
    Cubic,
    Constant,
    Step,
    Max
};

enum class EAnimationBlendMode : std::uint8_t
{
    None,
    Linear,
    Additive,
    Masked,
    TimeBased,
    Max
};

enum class ETangentMode : std::uint8_t
{
    None,
    NoneTangent,
    Auto,
    User,
    Break,
    Max
};

enum class ETangentWeightMode : std::uint8_t
{
    None,
    NoneWeight,
    Arrive,
    Leave,
    Both,
    Max
};

/*
*FbxTime
*FbxAnimLayer
*FbxAnimStack
 */

struct FCurve
{
    float Time;// 키프레임의 시간
    float Value;// 키프레임의 값
    
    float ArriveTangent;// 들어오는 탄젠트 (이전 키프레임에서 현재 키프레임으로의 기울기)
    float LeaveTangent;// 나가는 탄젠트 (현재 키프레임에서 다음 키프레임으로의 기울기)
    
    // 탄젠트의 가중치 (탄젠트의 영향력을 제어)
    float TangentWeightArrive;
    float TangentWeightLeave;
    
    EAnimInterpolationType InterpMode;    // 이 키프레임에 사용되는 보간 모드
    ETangentMode TangentMode;
    ETangentWeightMode TangentWeightMode;

    float Evaluate(float CurrentTime) const;

    void Serialize(FArchive& Ar) const
    {
        Ar << Time << Value << ArriveTangent << LeaveTangent << TangentWeightArrive << TangentWeightLeave;
        Ar << static_cast<uint32>(InterpMode) << static_cast<uint32>(TangentMode) << static_cast<uint32>(TangentWeightMode);
    }

    void Deserialize(FArchive& Ar)
    {
        uint32 interpMode, tangentMode, tangentWeightMode;
        Ar >> Time >> Value >> ArriveTangent >> LeaveTangent >> TangentWeightArrive >> TangentWeightLeave;
        Ar >> interpMode >> tangentMode >> tangentWeightMode;
        InterpMode = static_cast<EAnimInterpolationType>(interpMode);
        TangentMode = static_cast<ETangentMode>(tangentMode);
        TangentWeightMode = static_cast<ETangentWeightMode>(tangentWeightMode);
    }
};

struct FAnimationCurveData
{
    FName CurveName;
    TArray<FCurve> CurveWeights;

    void Serialize(FArchive& Ar) const
    {
        Ar << CurveName << CurveWeights;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> CurveName >> CurveWeights;
    }
};

struct FAnimNotifyEvent
{
    float TriggerTime;
    float Duration;
    FName NotifyName;
    class UAnimNotify* Notify;
    // < 연산자 오버로딩
    bool operator<(const FAnimNotifyEvent& other) const {
        return TriggerTime < other.TriggerTime;
    }
};

struct FRawAnimSequenceTrack
{
    TArray<FVector> PosKeys;        // 위치 키들
    TArray<FQuat> RotKeys;          // 회전 키들 
    TArray<FVector> ScaleKeys;      // 스케일 키들
    
    TArray<float> KeyTimes;         // 각 키프레임의 시간값

    EAnimInterpolationType InterpMode;     // 전체 트랙의 보간 모드

    void Serialize(FArchive& Ar) const;

    void Deserialize(FArchive& Ar);
};


struct FBoneAnimationTrack
{
    FName Name = "Empty";                       // Bone 이름
    FRawAnimSequenceTrack InternalTrackData;    // 실제 애니메이션 데이터

    void Serialize(FArchive& Ar) const
    {
        Ar << Name << InternalTrackData;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> Name >> InternalTrackData;
    }
};

struct FSkeletalAnimation
{
    FName Name = "Empty";
    TArray<FBoneAnimationTrack> BoneAnimTracks;
    TArray<FAnimationCurveData> AttributeCurves;
    TArray<FAnimNotifyEvent> Notifies;

    void Serialize(FArchive& Ar) const
    {
        Ar << Name << BoneAnimTracks << AttributeCurves;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> Name >> BoneAnimTracks >> AttributeCurves;
    }
};