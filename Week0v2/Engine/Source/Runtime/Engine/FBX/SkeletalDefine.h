#pragma once
#include "Define.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "Math/Matrix.h"
#include "Math/Quat.h"
#include "Math/Vector.h"

#pragma region Skeletal;
struct FBone
{
    FString BoneName;
    FMatrix SkinningMatrix;
    FMatrix InverseBindPoseMatrix;
    FMatrix GlobalTransform;
    FMatrix LocalTransform;
    int ParentIndex;
};

struct FBoneNode
{
    FString BoneName;
    int BoneIndex;             // Index in the Bones array
    TArray<int> ChildIndices;  // Indices of child bones
};

struct FSkeletalVertex
{
    FVector4 Position;
    FVector Normal;
    FVector Tangent;
    FVector2D TexCoord;
    int32 BoneIndices[4];
    float BoneWeights[4];

    void SkinningVertex(const TArray<FBone>& bones);
private:
    FVector SkinVertexPosition(const TArray<FBone>& bones) const;
};


struct FRefSkeletal
{
    // Tree structure for bones
    FString Name;
    TArray<FSkeletalVertex> RawVertices;
    TArray<FBone> RawBones;
    TArray<FBoneNode> BoneTree;
    TArray<int> RootBoneIndices;  // Indices of root bones (no parents)
    TMap<FString, int> BoneNameToIndexMap;  // For quick lookups
    TArray<UMaterial*> Materials;
    TArray<FMaterialSubset> MaterialSubsets;
};

struct FSkeletalMeshRenderData
{
    FString Name = "Empty";
    TArray<FSkeletalVertex> Vertices;
    TArray<uint32> Indices;
    TArray<FBone> Bones;
    FBoundingBox BoundingBox;
    ID3D11Buffer* VB = nullptr;
    ID3D11Buffer* IB = nullptr;
};
#pragma endregion

#pragma region Animation;
enum class EAnimInterpMode : std::uint8_t
{
    None,
    Linear,
    Cubic,
    Constant,
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

struct FCurveKey
{
    float Time;// 키프레임의 시간
    float Value;// 키프레임의 값
    
    float ArriveTangent;// 들어오는 탄젠트 (이전 키프레임에서 현재 키프레임으로의 기울기)
    float LeaveTangent;// 나가는 탄젠트 (현재 키프레임에서 다음 키프레임으로의 기울기)
    
    // 탄젠트의 가중치 (탄젠트의 영향력을 제어)
    float TangentWeightArrive;
    float TangentWeightLeave;
    
    EAnimInterpMode InterpMode;    // 이 키프레임에 사용되는 보간 모드
    ETangentMode TangentMode;
    ETangentWeightMode TangentWeightMode;
};

struct FAnimCurve
{
    FName CurveName;
    TArray<FCurveKey> KeyFrameInfo;
};

struct FAnimNotifyEvent
{
    float TriggerTime;
    float Duration;
    FName NotifyName;
};

struct FRawAnimSequenceTrack
{
    TArray<FVector> PosKeys;        // 위치 키들
    TArray<FQuat> RotKeys;          // 회전 키들 
    TArray<FVector> ScaleKeys;      // 스케일 키들
    
    TArray<float> KeyTimes;         // 각 키프레임의 시간값

    EAnimInterpMode InterpMode;     // 전체 트랙의 보간 모드
};

struct FBoneAnimationTrack
{
    FName Name;                             // Bone 이름
    FRawAnimSequenceTrack InternalTrack;    // 실제 애니메이션 데이터
};

struct FSkeletalAnimation
{
    TArray<FBoneAnimationTrack> BoneAnimTracks;
    TArray<FAnimCurve> AttributeCurves;
    TArray<FAnimNotifyEvent> Notifies;
};

#pragma endregion;