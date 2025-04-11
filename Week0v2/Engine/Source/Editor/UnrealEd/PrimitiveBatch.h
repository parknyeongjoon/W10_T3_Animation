#pragma once
#include "Define.h"
#include "Windows/D3D11RHI/CBStructDefine.h"

class UPrimitiveBatch
{
public:
    UPrimitiveBatch();
    ~UPrimitiveBatch();
    static UPrimitiveBatch& GetInstance() {
        static UPrimitiveBatch instance;
        return instance;
    }

public:
    void Release();
    void ClearGrid() {};
    float GetSpacing() { return GridParam.GridSpacing; }
    void GenerateGrid(float spacing, int gridCount);

    void AddAABB(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix);
    void AddOBB(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix);
    void AddCone(const FVector& center, float radius, float height, int segments,const FVector4& color, const FMatrix& modelMatri);

    void ClearBatchPrimitives() { BoundingBoxes.Empty(); OrientedBoundingBoxes.Empty(); Cones.Empty(); }
    
    // 복사 생성자 및 대입 연산자 삭제
    UPrimitiveBatch(const UPrimitiveBatch&) = delete;
    UPrimitiveBatch& operator=(const UPrimitiveBatch&) = delete;

    FGridParametersData GetGridParameters() const { return GridParam; }

    TArray<FBoundingBox>& GetBoundingBoxes() { return BoundingBoxes; }
    TArray<FOBB>& GetOrientedBoundingBoxes() { return OrientedBoundingBoxes; }
    TArray<FCone>& GetCones() { return Cones; }

    void SetGridParameters(const FGridParametersData& gridParam) { GridParam = gridParam; }
    void SetConeSegmentCount(const int count) { ConeSegmentCount = count; }
    int GetConeSegmentCount() const { return ConeSegmentCount; }

    size_t GetAllocatedBoundingBoxCapacity() const { return allocatedBoundingBoxCapacity; }
    size_t GetAllocatedConeCapacity() const { return allocatedConeCapacity; }
    size_t GetAllocatedOBBCapacity() const { return allocatedOBBCapacity; }

    void SetAllocatedBoundingBoxCapacity(const size_t capacity) { allocatedBoundingBoxCapacity = capacity; }
    void SetAllocatedConeCapacity(const size_t capacity) { allocatedConeCapacity = capacity; }
    void SetAllocatedOBBCapacity(const size_t capacity) { allocatedOBBCapacity = capacity; }
    
private:
    
    size_t allocatedBoundingBoxCapacity;
    size_t allocatedConeCapacity;
    size_t allocatedOBBCapacity;
    TArray<FBoundingBox> BoundingBoxes;
    TArray<FOBB> OrientedBoundingBoxes;
    TArray<FCone> Cones;
    FGridParametersData GridParam;
    int ConeSegmentCount = 0;
};