#include "StaticMeshComponent.h"

#include "Engine/World.h"
#include "Launch/EditorEngine.h"
#include "UObject/ObjectFactory.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "Classes/Engine/FObjLoader.h"
#include "Engine/StaticMesh.h"

UStaticMeshComponent::UStaticMeshComponent(const UStaticMeshComponent& Other)
    : UMeshComponent(Other)
    , StaticMesh(Other.StaticMesh)
    , selectedSubMeshIndex(Other.selectedSubMeshIndex)
{
}
uint32 UStaticMeshComponent::GetNumMaterials() const
{
    if (StaticMesh == nullptr) return 0;

    return StaticMesh->GetMaterials().Num();
}

UMaterial* UStaticMeshComponent::GetMaterial(uint32 ElementIndex) const
{
    if (StaticMesh != nullptr)
    {
        if (OverrideMaterials[ElementIndex] != nullptr)
        {
            return OverrideMaterials[ElementIndex];
        }
    
        if (StaticMesh->GetMaterials().IsValidIndex(ElementIndex))
        {
            return StaticMesh->GetMaterials()[ElementIndex]->Material;
        }
    }
    return nullptr;
}

uint32 UStaticMeshComponent::GetMaterialIndex(FName MaterialSlotName) const
{
    if (StaticMesh == nullptr) return -1;

    return StaticMesh->GetMaterialIndex(MaterialSlotName);
}

TArray<FName> UStaticMeshComponent::GetMaterialSlotNames() const
{
    TArray<FName> MaterialNames;
    if (StaticMesh == nullptr) return MaterialNames;

    for (const FStaticMaterial* Material : StaticMesh->GetMaterials())
    {
        MaterialNames.Emplace(Material->MaterialSlotName);
    }

    return MaterialNames;
}

void UStaticMeshComponent::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    if (StaticMesh == nullptr) return;
    StaticMesh->GetUsedMaterials(Out);
    for (int materialIndex = 0; materialIndex < GetNumMaterials(); materialIndex++)
    {
        if (OverrideMaterials[materialIndex] != nullptr)
        {
            Out[materialIndex] = OverrideMaterials[materialIndex];
        }
    }
}

int UStaticMeshComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    if (!AABB.IntersectRay(rayOrigin, rayDirection, pfNearHitDistance)) return 0;
    int nIntersections = 0;
    if (StaticMesh == nullptr) return 0;

    OBJ::FStaticMeshRenderData* renderData = StaticMesh->GetRenderData();

    FVertexSimple* vertices = renderData->Vertices.GetData();
    int vCount = renderData->Vertices.Num();
    UINT* indices = renderData->Indices.GetData();
    int iCount = renderData->Indices.Num();

    if (!vertices) return 0;
    BYTE* pbPositions = reinterpret_cast<BYTE*>(renderData->Vertices.GetData());

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
        uint32 stride = sizeof(FVertexSimple);
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


void UStaticMeshComponent::SetStaticMesh(UStaticMesh* value)
{ 
    StaticMesh = value;
    OverrideMaterials.SetNum(value->GetMaterials().Num());
    AABB = FBoundingBox(StaticMesh->GetRenderData()->BoundingBoxMin, StaticMesh->GetRenderData()->BoundingBoxMax);
    VBIBTopologyMappingName = StaticMesh->GetRenderData()->DisplayName;
}

std::unique_ptr<FActorComponentInfo> UStaticMeshComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FStaticMeshComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void UStaticMeshComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FStaticMeshComponentInfo* Info = static_cast<FStaticMeshComponentInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);

    Info->StaticMeshPath = StaticMesh->GetRenderData()->PathName;
}
void UStaticMeshComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);

    const FStaticMeshComponentInfo& StaticMeshInfo = static_cast<const FStaticMeshComponentInfo&>(Info);
    UStaticMesh* Mesh = FManagerOBJ::CreateStaticMesh(FString::ToFString(StaticMeshInfo.StaticMeshPath));
    SetStaticMesh(Mesh);

}
UObject* UStaticMeshComponent::Duplicate() const
{
    UStaticMeshComponent* NewComp = FObjectFactory::ConstructObjectFrom<UStaticMeshComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void UStaticMeshComponent::DuplicateSubObjects(const UObject* Source)
{
    UMeshComponent::DuplicateSubObjects(Source);
}

void UStaticMeshComponent::PostDuplicate() {}

void UStaticMeshComponent::TickComponent(float DeltaTime)
{
    //Timer += DeltaTime * 0.005f;
    //SetLocation(GetWorldLocation()+ (FVector(1.0f,1.0f, 1.0f) * sin(Timer)));
}
