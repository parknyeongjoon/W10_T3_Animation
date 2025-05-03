#pragma once
#include "Container/Array.h"
#include "UObject/NameTypes.h"

struct FQuat;
struct FVector;
// This contains Reference-skeleton related info
// Bone transform is saved as FTransform array
struct FMeshBoneInfo
{
    // Bone's name.
    FName Name;

    // INDEX_NONE if this is the root bone. 
    int32 ParentIndex;

    // Name Inedx 0
    FMeshBoneInfo() : Name(), ParentIndex(-1) {}

    FMeshBoneInfo(const FName& InName, int32 InParentIndex)
        : Name(InName)
        , ParentIndex(InParentIndex)
    {}

    bool operator==(const FMeshBoneInfo& B) const
    {
        return (Name == B.Name);
    }

    // friend FArchive &operator<<(FArchive& Ar, FMeshBoneInfo& F);
};


class FReferenceSkeleton
{
public:


private:


    // 현재 Original Asset의 정보만을 저장함. (UE의 Raw, Final RefBone에 대해 참고)
    // 애니메이션도 제외됨.
    
    // 메타정보 (Bone Name, Parent Index)
    TArray<FMeshBoneInfo>	RawRefBoneInfo;

    // 각 본의 Local Space 기준 Transform 정보
    // Location, Rotation, Scale
    // TODO Transform 혹은 구조체 그에 상응하는 구조체 사용시 변경
    TArray<FVector>		RawRefBonePoseLocation;
    TArray<FQuat>		RawRefBonePoseRotation;
    TArray<FVector>		RawRefBonePoseScale;

    // Bone Name - Indexing Mapping Chache
    // TMap<FName, int32> RawNameToIndexMap;


    
};
