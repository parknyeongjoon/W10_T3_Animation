#include "SkeletalMeshComponent.h"

#include "LaunchEngineLoop.h"
#include "Engine/FBXLoader.h"
#include "Animation/AnimInstance.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Engine/World.h"
#include "Launch/EditorEngine.h"
#include "UObject/ObjectFactory.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "Classes/Engine/FLoaderOBJ.h"
#include "GameFramework/Actor.h"
#include "Math/JungleMath.h"
#include "Math/Transform.h"
#include "Renderer/Renderer.h"
#include "StaticMeshComponents/StaticMeshComponent.h"
#include "UObject/Casts.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/CustomAnimInstance/TestAnimInstance.h"

USkeletalMeshComponent::USkeletalMeshComponent(const USkeletalMeshComponent& Other)
    : UMeshComponent(Other)
    , SkeletalMesh(Other.SkeletalMesh)
    , SelectedSubMeshIndex(Other.SelectedSubMeshIndex)
{
}
uint32 USkeletalMeshComponent::GetNumMaterials() const
{
    if (SkeletalMesh == nullptr) return 0;

    return SkeletalMesh->GetMaterials().Num();
}

UMaterial* USkeletalMeshComponent::GetMaterial(uint32 ElementIndex) const
{
    if (SkeletalMesh != nullptr)
    {
        if (OverrideMaterials[ElementIndex] != nullptr)
        {
            return OverrideMaterials[ElementIndex];
        }
    
        if (SkeletalMesh->GetMaterials().IsValidIndex(ElementIndex))
        {
            return SkeletalMesh->GetMaterials()[ElementIndex]->Material;
        }
    }
    return nullptr;
}

uint32 USkeletalMeshComponent::GetMaterialIndex(FName MaterialSlotName) const
{
    if (SkeletalMesh == nullptr) return -1;

    return SkeletalMesh->GetMaterialIndex(MaterialSlotName);
}

TArray<FName> USkeletalMeshComponent::GetMaterialSlotNames() const
{
    TArray<FName> MaterialNames;
    if (SkeletalMesh == nullptr) return MaterialNames;

    for (const FMaterialSlot* Material : SkeletalMesh->GetMaterials())
    {
        MaterialNames.Emplace(Material->MaterialSlotName);
    }

    return MaterialNames;
}

void USkeletalMeshComponent::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    if (SkeletalMesh == nullptr) return;
    SkeletalMesh->GetUsedMaterials(Out);
    for (int materialIndex = 0; materialIndex < GetNumMaterials(); materialIndex++)
    {
        if (OverrideMaterials[materialIndex] != nullptr)
        {
            Out[materialIndex] = OverrideMaterials[materialIndex];
        }
    }
}

int USkeletalMeshComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    if (!AABB.IntersectRay(rayOrigin, rayDirection, pfNearHitDistance)) return 0;
    int nIntersections = 0;
    if (SkeletalMesh == nullptr) return 0;

    FSkeletalMeshRenderData renderData = SkeletalMesh->GetRenderData();

    FSkeletalVertex* vertices = renderData.Vertices.GetData();
    int vCount = renderData.Vertices.Num();
    UINT* indices = renderData.Indices.GetData();
    int iCount = renderData.Indices.Num();

    if (!vertices) return 0;
    BYTE* pbPositions = reinterpret_cast<BYTE*>(renderData.Vertices.GetData());

    int nPrimitives = (!indices) ? (vCount / 3) : (iCount / 3);
    float fNearHitDistance = FLT_MAX;
    for (int i = 0; i < nPrimitives; i++) {
        int idx0, idx1, idx2;
        if (!indices) {
            idx0 = i * 3;
            idx1 = i * 3 + 1;
            idx2 = i * 3 + 2;
        }
        else {
            idx0 = indices[i * 3];
            idx2 = indices[i * 3 + 1];
            idx1 = indices[i * 3 + 2];
        }

        // 각 삼각형의 버텍스 위치를 FVector로 불러옵니다.
        uint32 stride = sizeof(FSkeletalVertex);
        FVector v0 = *reinterpret_cast<FVector*>(pbPositions + idx0 * stride);
        FVector v1 = *reinterpret_cast<FVector*>(pbPositions + idx1 * stride);
        FVector v2 = *reinterpret_cast<FVector*>(pbPositions + idx2 * stride);

        float fHitDistance;
        if (IntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, fHitDistance)) {
            if (fHitDistance < fNearHitDistance) {
                pfNearHitDistance = fNearHitDistance = fHitDistance;
            }
            nIntersections++;
        }

    }
    return nIntersections;
}


void USkeletalMeshComponent::SetSkeletalMesh(USkeletalMesh* value)
{ 
    SkeletalMesh = value;
    VBIBTopologyMappingName = SkeletalMesh->GetFName();
    value->UpdateBoneHierarchy();
    
    OverrideMaterials.SetNum(value->GetMaterials().Num());
    AABB = SkeletalMesh->GetRenderData().BoundingBox;

    // CreateBoneComponents();
}

UAnimSingleNodeInstance* USkeletalMeshComponent::GetSingleNodeInstance() const
{
    return Cast<UAnimSingleNodeInstance>(AnimInstance);
}

void USkeletalMeshComponent::CreateBoneComponents()
{
    // 이미 할당된 component가 있다면 삭제
    for (auto& BoneComp : BoneComponents)
    {
        BoneComp->DestroyComponent();
    }

    FManagerOBJ::CreateStaticMesh("Contents/helloBlender.obj");
    for (const auto& Bone : GetSkeletalMesh()->GetRenderData().Bones)
    {
        UStaticMeshComponent* BoneComp = GetOwner()->AddComponent<UStaticMeshComponent>(EComponentOrigin::Runtime);
        BoneComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"helloBlender.obj"));
        BoneComp->SetWorldLocation(Bone.GlobalTransform.GetTranslationVector());
        BoneComp->SetFName(Bone.BoneName);
        BoneComponents.Add(BoneComp);
    }
}

USkeletalMesh* USkeletalMeshComponent::LoadSkeletalMesh(const FString& FilePath)
{
    USkeletalMesh* SkeletalMesh = FFBXLoader::CreateSkeletalMesh(FilePath);
    SetSkeletalMesh(SkeletalMesh);

    return SkeletalMesh;
}

void USkeletalMeshComponent::UpdateBoneHierarchy()
{
    for (int i=0;i<SkeletalMesh->GetRenderData().Vertices.Num();i++)
    {
         SkeletalMesh->GetRenderData().Vertices[i].Position
        = SkeletalMesh->GetSkeleton()->GetRefSkeletal()->RawVertices[i].Position;
    }
    
    SkeletalMesh->UpdateBoneHierarchy();
    SkinningVertex();
}

void USkeletalMeshComponent::PlayAnimation(UAnimSequence* NewAnimToPlay, bool bLooping)
{
    SetAnimSequence(NewAnimToPlay);
    Play(bLooping);
}

void USkeletalMeshComponent::SetAnimSequence(UAnimSequence* NewAnimToPlay)
{
    if (NewAnimToPlay == nullptr)
    {
        return;
    }
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetCurrentSequence(NewAnimToPlay);
    }
}

UAnimSequence* USkeletalMeshComponent::GetAnimSequence() const
{

    return GetSingleNodeInstance()->GetCurrentSequence();
}

void USkeletalMeshComponent::SkinningVertex()
{
    for (auto& Vertex : SkeletalMesh->GetRenderData().Vertices)
    {
        Vertex.SkinningVertex(SkeletalMesh->GetRenderData().Bones);
    }

    FFBXLoader::UpdateBoundingBox(SkeletalMesh->GetRenderData());
    AABB = SkeletalMesh->GetRenderData().BoundingBox;

    SkeletalMesh->SetData(SkeletalMesh->GetRenderData(), SkeletalMesh->GetSkeleton()); // TODO: Dynamic VertexBuffer Update하게 바꾸기
}

// std::unique_ptr<FActorComponentInfo> USkeletalMeshComponent::GetComponentInfo()
// {
//     auto Info = std::make_unique<FStaticMeshComponentInfo>();
//     SaveComponentInfo(*Info);
//     
//     return Info;
// }

// void UStaticMeshComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
// {
//     FStaticMeshComponentInfo* Info = static_cast<FStaticMeshComponentInfo*>(&OutInfo);
//     Super::SaveComponentInfo(*Info);
//
//     Info->StaticMeshPath = staticMesh->GetRenderData()->PathName;
// }

// void UStaticMeshComponent::LoadAndConstruct(const FActorComponentInfo& Info)
// {
//     Super::LoadAndConstruct(Info);
//
//     const FStaticMeshComponentInfo& StaticMeshInfo = static_cast<const FStaticMeshComponentInfo&>(Info);
//     UStaticMesh* Mesh = FManagerOBJ::CreateStaticMesh(FString::ToFString(StaticMeshInfo.StaticMeshPath));
//     SetStaticMesh(Mesh);
// }

UObject* USkeletalMeshComponent::Duplicate(UObject* InOuter)
{
    USkeletalMeshComponent* NewComp = FObjectFactory::ConstructObjectFrom<USkeletalMeshComponent>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

void USkeletalMeshComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UMeshComponent::DuplicateSubObjects(Source, InOuter);
    // TODO: SkeletalMesh 복사
    SkeletalMesh = Cast<USkeletalMesh>(Cast<USkeletalMeshComponent>(Source)->SkeletalMesh->Duplicate(this));
    AnimInstance = Cast<UAnimInstance>(Cast<USkeletalMeshComponent>(Source)->AnimInstance->Duplicate(this));
}

void USkeletalMeshComponent::PostDuplicate() {}

void USkeletalMeshComponent::BeginPlay()
{
    UMeshComponent::BeginPlay();
    animTime = 0.f;
}

void USkeletalMeshComponent::TickComponent(float DeltaTime)
{
    if (AnimInstance) AnimInstance->NativeUpdateAnimation(DeltaTime);

    SkeletalMesh->UpdateBoneHierarchy();
    if (GEngineLoop.Renderer.GetSkinningMode() == ESkinningType::CPU)
    {
        SkeletalMesh->UpdateSkinnedVertices();
        bCPUSkinned = true; 
    }
    else
    {
        if (bCPUSkinned)
        {
            SkeletalMesh->ResetToOriginalPose();
            bCPUSkinned = false;
        }   
    }
}

void USkeletalMeshComponent::SetData(const FString& FilePath)
{
    SkeletalMesh = LoadSkeletalMesh(FilePath);
}

void USkeletalMesh::ResetToOriginalPose()
{
    // 본 트랜스폼 복원
    for (int i = 0; i < Skeleton->GetRefSkeletal()->RawVertices.Num() && i < SkeletalMeshRenderData.Bones.Num(); i++)
    {
        // 로컬 트랜스폼 복원
        SkeletalMeshRenderData.Bones[i].LocalTransform = Skeleton->GetRefSkeletal()->RawBones[i].LocalTransform;
        SkeletalMeshRenderData.Bones[i].GlobalTransform = Skeleton->GetRefSkeletal()->RawBones[i].GlobalTransform;
        SkeletalMeshRenderData.Bones[i].SkinningMatrix = Skeleton->GetRefSkeletal()->RawBones[i].SkinningMatrix;
    }

    // 스키닝 적용
    UpdateSkinnedVertices();
}

void USkeletalMeshComponent::Play(bool bLooping)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetPlaying(true);
        SingleNodeInstance->SetLooping(bLooping);
    }
}

void USkeletalMeshComponent::Stop()
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetPlaying(false);
    }
}

void USkeletalMeshComponent::SetPlaying(bool bPlaying)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetPlaying(bPlaying);
    }
}

bool USkeletalMeshComponent::IsPlaying() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->IsPlaying();
    }

    return false;
}

void USkeletalMeshComponent::SetReverse(bool bIsReverse)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetReverse(bIsReverse);
    }
}

bool USkeletalMeshComponent::IsReverse() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->IsReverse();
    }
}

void USkeletalMeshComponent::SetPlayRate(float Rate)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetPlayRate(Rate);
    }
}

float USkeletalMeshComponent::GetPlayRate() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->GetPlayRate();
    }

    return 0.f;
}

void USkeletalMeshComponent::SetLooping(bool bIsLooping)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetLooping(bIsLooping);
    }
}

bool USkeletalMeshComponent::IsLooping() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->IsLooping();
    }
    return false;
}

int USkeletalMeshComponent::GetCurrentKey() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->GetCurrentKey();
    }
    return 0;
}

void USkeletalMeshComponent::SetCurrentKey(int InKey)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetCurrentKey(InKey);
    }
}

void USkeletalMeshComponent::SetElapsedTime(float InElapsedTime)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetElapsedTime(InElapsedTime);
    }
}

float USkeletalMeshComponent::GetElapsedTime() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->GetElapsedTime();
    }
    return 0.f;
}

int32 USkeletalMeshComponent::GetLoopStartFrame() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->GetLoopStartFrame();
    }
    return 0;
}

void USkeletalMeshComponent::SetLoopStartFrame(int32 InLoopStartFrame)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetLoopStartFrame(InLoopStartFrame);
    }
}

int32 USkeletalMeshComponent::GetLoopEndFrame() const
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        return SingleNodeInstance->GetLoopEndFrame();
    }
    return 0;
}

void USkeletalMeshComponent::SetLoopEndFrame(int32 InLoopEndFrame)
{
    if (UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance())
    {
        SingleNodeInstance->SetLoopEndFrame(InLoopEndFrame);
    }
}

void USkeletalMeshComponent::SetAnimationMode(EAnimationMode InAnimationMode)
{
    AnimationMode = InAnimationMode;
}
