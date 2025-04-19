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
    void AddCone(const FVector& start, float radius, const FVector& end, int segments, const FVector4& color);
    void AddSphere(const FVector& center, float radius, const FVector4& color);
    void AddLine(const FVector& StartPos, const FVector& Direction, const float Length, const FVector4& Color);

    void ClearBatchPrimitives() { BoundingBoxes.Empty(); OrientedBoundingBoxes.Empty(); Cones.Empty(); Spheres.Empty(); Lines.Empty(); }
    
    // 복사 생성자 및 대입 연산자 삭제
    UPrimitiveBatch(const UPrimitiveBatch&) = delete;
    UPrimitiveBatch& operator=(const UPrimitiveBatch&) = delete;

    FGridParametersData GetGridParameters() const { return GridParam; }

    TArray<FBoundingBox>& GetBoundingBoxes() { return BoundingBoxes; }
    TArray<FOBB>& GetOrientedBoundingBoxes() { return OrientedBoundingBoxes; }
    TArray<FCone>& GetCones() { return Cones; }
    TArray<FSphere>& GetSpheres() { return Spheres; }
    TArray<FLine>& GetLines() { return Lines; }

    void SetGridParameters(const FGridParametersData& gridParam) { GridParam = gridParam; }
    void SetConeSegmentCount(const int count) { ConeSegmentCount = count; }
    int GetConeSegmentCount() const { return ConeSegmentCount; }

    size_t GetAllocatedBoundingBoxCapacity() const { return allocatedBoundingBoxCapacity; }
    size_t GetAllocatedConeCapacity() const { return allocatedConeCapacity; }
    size_t GetAllocatedOBBCapacity() const { return allocatedOBBCapacity; }
    size_t GetAllocatedSphereCapacity() const { return allocatedSphereCapacity; }
    size_t GetAllocatedLineCapacity() const { return allocatedLineCapacity; }

    void SetAllocatedBoundingBoxCapacity(const size_t capacity) { allocatedBoundingBoxCapacity = capacity; }
    void SetAllocatedConeCapacity(const size_t capacity) { allocatedConeCapacity = capacity; }
    void SetAllocatedOBBCapacity(const size_t capacity) { allocatedOBBCapacity = capacity; }
    void SetAllocatedSphereCapacity(const size_t capacity) { allocatedSphereCapacity = capacity; }
    void SetAllocatedLineCapacity(const size_t capacity) { allocatedLineCapacity = capacity; }
    
private:
    
    size_t allocatedBoundingBoxCapacity;
    size_t allocatedConeCapacity;
    size_t allocatedOBBCapacity;
    size_t allocatedSphereCapacity;
    size_t allocatedLineCapacity;
    TArray<FBoundingBox> BoundingBoxes;
    TArray<FOBB> OrientedBoundingBoxes;
    TArray<FCone> Cones;
    TArray<FSphere> Spheres;
    TArray<FLine> Lines;
    FGridParametersData GridParam;
    int ConeSegmentCount = 0;
};