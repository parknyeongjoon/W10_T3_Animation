#include "SkeletalMesh.h"

#include "Renderer/RenderResourceManager.h"
#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"
#include "TestFBXLoader.h"
#include "Components/Material/Material.h"
#include "Engine/FLoaderOBJ.h"

uint32 USkeletalMesh::GetMaterialIndex(FName MaterialSlotName) const
{
    for (uint32 materialIndex = 0; materialIndex < MaterialSlots.Num(); materialIndex++) {
        if (MaterialSlots[materialIndex]->MaterialSlotName == MaterialSlotName)
            return materialIndex;
    }

    return -1;
}

void USkeletalMesh::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    for (const FMaterialSlot* Material : MaterialSlots)
    {
        Out.Emplace(Material->Material);
    }
}

void USkeletalMesh::SetData(const FString& FilePath)
{
    FSkeletalMeshRenderData SkeletalMeshRenderData = TestFBXLoader::GetCopiedSkeletalRenderData(FilePath);
    FRefSkeletal* RefSkeletal = TestFBXLoader::GetRefSkeletal(FilePath);

    SetData(SkeletalMeshRenderData, RefSkeletal);
}

void USkeletalMesh::SetData(const FSkeletalMeshRenderData& InRenderData, FRefSkeletal* InRefSkeletal)
{
    SkeletalMeshRenderData = InRenderData;
    RefSkeletal = InRefSkeletal;

    ID3D11Buffer* VB = nullptr; 
    ID3D11Buffer* IB = nullptr;

    const uint32 verticeNum = SkeletalMeshRenderData.Vertices.Num();
    if (verticeNum <= 0) return;

    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    VB = renderResourceManager->CreateDynamicVertexBuffer<FSkeletalVertex>(SkeletalMeshRenderData.Vertices);
    renderResourceManager->AddOrSetVertexBuffer(SkeletalMeshRenderData.Name, VB);
    GEngineLoop.Renderer.MappingVBTopology(SkeletalMeshRenderData.Name, SkeletalMeshRenderData.Name, sizeof(FSkeletalVertex), verticeNum);
    const uint32 indexNum = SkeletalMeshRenderData.Indices.Num();
    if (indexNum > 0)
    {
        IB = renderResourceManager->CreateIndexBuffer(SkeletalMeshRenderData.Indices);
        renderResourceManager->AddOrSetIndexBuffer(SkeletalMeshRenderData.Name, IB);
    }
    GEngineLoop.Renderer.MappingIB(SkeletalMeshRenderData.Name, SkeletalMeshRenderData.Name, indexNum);

    MaterialSlots.Empty();
    for (int materialIndex = 0; materialIndex < RefSkeletal->Materials.Num(); materialIndex++) {
        FMaterialSlot* newMaterialSlot = new FMaterialSlot();
        UMaterial* newMaterial = FManagerOBJ::CreateMaterial(RefSkeletal->Materials[materialIndex]->GetMaterialInfo());

        newMaterialSlot->Material = newMaterial;
        newMaterialSlot->MaterialSlotName = RefSkeletal->Materials[materialIndex]->GetMaterialInfo().MTLName;

        MaterialSlots.Add(newMaterialSlot);
    }
}

void USkeletalMesh::UpdateBoneHierarchy()
{
    // 먼저 루트 뼈들의 글로벌 트랜스폼을 설정
    for (int32 RootIndex : RefSkeletal->RootBoneIndices)
    {
        // 루트 뼈는 로컬 트랜스폼이 곧 글로벌 트랜스폼이 됨
        SkeletalMeshRenderData.Bones[RootIndex].GlobalTransform
        = SkeletalMeshRenderData.Bones[RootIndex].LocalTransform;

        SkeletalMeshRenderData.Bones[RootIndex].SkinningMatrix
        = SkeletalMeshRenderData.Bones[RootIndex].InverseBindPoseMatrix
        * SkeletalMeshRenderData.Bones[RootIndex].GlobalTransform;
        
        // 재귀적으로 자식 뼈들의 글로벌 트랜스폼을 업데이트
        UpdateChildBones(RootIndex);
    }
}

void USkeletalMesh::UpdateChildBones(int ParentIndex)
{
    // BoneTree 구조를 사용하여 현재 부모 뼈의 모든 자식을 찾음
    const FBoneNode& ParentNode = RefSkeletal->BoneTree[ParentIndex];
    
    // 모든 자식 뼈를 순회
    for (int32 ChildIndex : ParentNode.ChildIndices)
    {
        // 자식의 글로벌 트랜스폼은 부모의 글로벌 트랜스폼과 자식의 로컬 트랜스폼을 결합한 것
        SkeletalMeshRenderData.Bones[ChildIndex].GlobalTransform
        = SkeletalMeshRenderData.Bones[ChildIndex].LocalTransform
        * SkeletalMeshRenderData.Bones[ParentIndex].GlobalTransform;

        SkeletalMeshRenderData.Bones[ChildIndex].SkinningMatrix
        = SkeletalMeshRenderData.Bones[ChildIndex].InverseBindPoseMatrix
        * SkeletalMeshRenderData.Bones[ChildIndex].GlobalTransform;
        
        // 재귀적으로 이 자식의 자식들도 업데이트
        UpdateChildBones(ChildIndex);
    }
}

// USkeletalMesh 클래스에 추가할 함수
void USkeletalMesh::RotateBoneByName(const FString& BoneName, float AngleInDegrees, const FVector& RotationAxis)
{
    // 이름으로 본 인덱스 찾기
    int targetBoneIndex = FindBoneIndexByName(BoneName);

    // 본을 찾지 못한 경우
    if (targetBoneIndex < 0)
        return;

    // 회전 행렬 생성 및 적용
    ApplyRotationToBone(targetBoneIndex, AngleInDegrees, RotationAxis);

    // 계층 구조 업데이트
    UpdateBoneHierarchy();

    // 스키닝 적용 및 버퍼 업데이트
    UpdateSkinnedVertices();
}

int USkeletalMesh::FindBoneIndexByName(const FString& BoneName) const
{
    if (SkeletalMeshRenderData.Bones.Num() == 0)
        return -1;

    for (int i = 0; i < SkeletalMeshRenderData.Bones.Num(); i++)
    {
        if (SkeletalMeshRenderData.Bones[i].BoneName == BoneName)
            return i;
    }

    return -1;
}

void USkeletalMesh::ApplyRotationToBone(int BoneIndex, float AngleInDegrees, const FVector& RotationAxis)
{
    if (BoneIndex < 0 || BoneIndex >= SkeletalMeshRenderData.Bones.Num())
        return;

    float angleInRadians = AngleInDegrees * 3.14159f / 180.0f;
    FMatrix rotationMatrix = FMatrix::Identity;

    // X축 회전
    if (RotationAxis.X != 0.0f)
    {
        rotationMatrix.M[1][1] = cos(angleInRadians);
        rotationMatrix.M[1][2] = -sin(angleInRadians);
        rotationMatrix.M[2][1] = sin(angleInRadians);
        rotationMatrix.M[2][2] = cos(angleInRadians);
    }
    // Y축 회전
    else if (RotationAxis.Y != 0.0f)
    {
        rotationMatrix.M[0][0] = cos(angleInRadians);
        rotationMatrix.M[0][2] = sin(angleInRadians);
        rotationMatrix.M[2][0] = -sin(angleInRadians);
        rotationMatrix.M[2][2] = cos(angleInRadians);
    }
    // Z축 회전
    else if (RotationAxis.Z != 0.0f)
    {
        rotationMatrix.M[0][0] = cos(angleInRadians);
        rotationMatrix.M[0][1] = -sin(angleInRadians);
        rotationMatrix.M[1][0] = sin(angleInRadians);
        rotationMatrix.M[1][1] = cos(angleInRadians);
    }

    // 본 로컬 변환에 회전 적용
    SkeletalMeshRenderData.Bones[BoneIndex].LocalTransform =
        rotationMatrix * SkeletalMeshRenderData.Bones[BoneIndex].LocalTransform;
}

void USkeletalMesh::ProcessBoneRotationInput(float DeltaTime)
{
    if (!this)
        return;

    // 선택된 본이 없으면 첫 번째 본 사용
    if (CurrentSelectedBone.IsEmpty() && GetRenderData().Bones.Num() > 0)
    {
        CurrentSelectedBone = GetRenderData().Bones[0].BoneName;
        std::cout << "선택된 본:" << CurrentSelectedBone;
    }

    // 회전 속도 설정 (초당 각도)
    float rotationSpeed = 1.0f * DeltaTime;

    // 키 입력 처리 - 엔진의 입력 시스템에 맞게 조정 필요
    // X축 회전 (Q/A 키)
    if (GetAsyncKeyState('Q') & 0x8000) // Windows API 사용 예시
    {
        this->RotateBoneByName(CurrentSelectedBone, rotationSpeed, FVector(1, 0, 0));
    }
    else if (GetAsyncKeyState('A') & 0x8000)
    {
        this->RotateBoneByName(CurrentSelectedBone, -rotationSpeed, FVector(1, 0, 0));
    }

    // Y축 회전 (W/S 키)
    if (GetAsyncKeyState('W') & 0x8000)
    {
        this->RotateBoneByName(CurrentSelectedBone, rotationSpeed, FVector(0, 1, 0));
    }
    else if (GetAsyncKeyState('S') & 0x8000)
    {
        this->RotateBoneByName(CurrentSelectedBone, -rotationSpeed, FVector(0, 1, 0));
    }

    // Z축 회전 (E/D 키)
    if (GetAsyncKeyState('E') & 0x8000)
    {
        this->RotateBoneByName(CurrentSelectedBone, rotationSpeed, FVector(0, 0, 1));
    }
    else if (GetAsyncKeyState('D') & 0x8000)
    {
        this->RotateBoneByName(CurrentSelectedBone, -rotationSpeed, FVector(0, 0, 1));
    }

    // 본 선택 (숫자 키)
    for (int i = 0; i < 9 && i < this->GetRenderData().Bones.Num(); i++)
    {
        if (GetAsyncKeyState('1' + i) & 0x8000)
        {
            CurrentSelectedBone = this->GetRenderData().Bones[i].BoneName;
            std::cout << "선택된 본 :" << CurrentSelectedBone;
            break;
        }
    }
}

void USkeletalMesh::UpdateSkinnedVertices()
{
    if (SkeletalMeshRenderData.Vertices.Num() <= 0)
        return;

    // 스키닝 적용
    for (auto& Vertex : SkeletalMeshRenderData.Vertices)
    {
        Vertex.SkinningVertex(SkeletalMeshRenderData.Bones);
    }

    // 버퍼 업데이트
    UpdateVertexBuffer();
}

void USkeletalMesh::UpdateVertexBuffer()
{
    if (SkeletalMeshRenderData.Vertices.Num() <= 0)
        return;

    // 버텍스 버퍼 업데이트 - 이미 SetData에서 처리되므로 여기서는 간단히 호출
    SetData(SkeletalMeshRenderData, RefSkeletal);
}

