#include "PrimitiveBatch.h"
#include "EditorEngine.h"
#include "UnrealEd/EditorViewportClient.h"
extern UEditorEngine* GEngine;

UPrimitiveBatch::UPrimitiveBatch()
{
    GenerateGrid(5, 5000);
}

UPrimitiveBatch::~UPrimitiveBatch()
{
}

void UPrimitiveBatch::Release()
{
}

void UPrimitiveBatch::GenerateGrid(float spacing, int gridCount)
{
    GridParam.GridSpacing = spacing;
    GridParam.GridCount = gridCount;
    GridParam.GridOrigin = { 0,0,0 };
}

// void UPrimitiveBatch::RenderBatch(ID3D11Buffer* ConstantBuffer, const FMatrix& View, const FMatrix& Projection)
// {
//     UEditorEngine::renderer.PrepareLineShader();
//
//     InitializeVertexBuffer();
//
//     FMatrix Model = FMatrix::Identity;
//     FMatrix ViewProj = View * Projection;
//     FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
//     UEditorEngine::renderer.GetConstantBufferUpdater().UpdateConstant(ConstantBuffer, Model, ViewProj, NormalMatrix, FVector4(0, 0, 0, 0), false);
//     UEditorEngine::renderer.UpdateGridConstantBuffer(GridParam);
//
//     UpdateBoundingBoxResources();
//     UpdateConeResources();
//     UpdateOBBResources();
//     int boundingBoxSize = static_cast<int>(BoundingBoxes.Num());
//     int coneSize = static_cast<int>(Cones.Num());
//     int obbSize = static_cast<int>(OrientedBoundingBoxes.Num());
//     UEditorEngine::renderer.UpdateLinePrimitveCountBuffer(boundingBoxSize, coneSize);
//     UEditorEngine::renderer.RenderBatch(GridParam, pVertexBuffer, boundingBoxSize, coneSize, ConeSegmentCount, obbSize);
//     BoundingBoxes.Empty();
//     Cones.Empty();
//     OrientedBoundingBoxes.Empty();
//     UEditorEngine::renderer.PrepareShader();
// }

void UPrimitiveBatch::AddAABB(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix)
{
    FVector localVertices[8] = {
         { localAABB.min.x, localAABB.min.y, localAABB.min.z },
         { localAABB.max.x, localAABB.min.y, localAABB.min.z },
         { localAABB.min.x, localAABB.max.y, localAABB.min.z },
         { localAABB.max.x, localAABB.max.y, localAABB.min.z },
         { localAABB.min.x, localAABB.min.y, localAABB.max.z },
         { localAABB.max.x, localAABB.min.y, localAABB.max.z },
         { localAABB.min.x, localAABB.max.y, localAABB.max.z },
         { localAABB.max.x, localAABB.max.y, localAABB.max.z }
    };

    FVector worldVertices[8];
    worldVertices[0] = center + FMatrix::TransformVector(localVertices[0], modelMatrix);

    FVector min = worldVertices[0], max = worldVertices[0];

    // 첫 번째 값을 제외한 나머지 버텍스를 변환하고 min/max 계산
    for (int i = 1; i < 8; ++i)
    {
        worldVertices[i] = center + FMatrix::TransformVector(localVertices[i], modelMatrix);

        min.x = (worldVertices[i].x < min.x) ? worldVertices[i].x : min.x;
        min.y = (worldVertices[i].y < min.y) ? worldVertices[i].y : min.y;
        min.z = (worldVertices[i].z < min.z) ? worldVertices[i].z : min.z;

        max.x = (worldVertices[i].x > max.x) ? worldVertices[i].x : max.x;
        max.y = (worldVertices[i].y > max.y) ? worldVertices[i].y : max.y;
        max.z = (worldVertices[i].z > max.z) ? worldVertices[i].z : max.z;
    }
    FBoundingBox BoundingBox;
    BoundingBox.min = min;
    BoundingBox.max = max;
    BoundingBoxes.Add(BoundingBox);
}
void UPrimitiveBatch::AddOBB(const FBoundingBox& localAABB, const FVector& center, const FMatrix& modelMatrix)
{
    // 1) 로컬 AABB의 8개 꼭짓점
    FVector localVertices[8] =
    {
        { localAABB.min.x, localAABB.min.y, localAABB.min.z },
        { localAABB.max.x, localAABB.min.y, localAABB.min.z },
        { localAABB.min.x, localAABB.max.y, localAABB.min.z },
        { localAABB.max.x, localAABB.max.y, localAABB.min.z },
        { localAABB.min.x, localAABB.min.y, localAABB.max.z },
        { localAABB.max.x, localAABB.min.y, localAABB.max.z },
        { localAABB.min.x, localAABB.max.y, localAABB.max.z },
        { localAABB.max.x, localAABB.max.y, localAABB.max.z }
    };

    FOBB faceBB;
    for (int32 i = 0; i < 8; ++i) {
        // 모델 매트릭스로 점을 변환 후, center를 더해준다.
        faceBB.corners[i] =  center + FMatrix::TransformVector(localVertices[i], modelMatrix);
    }

    OrientedBoundingBoxes.Add(faceBB);

}

void UPrimitiveBatch::AddCone(const FVector& start, float radius, const FVector& end, int segments, const FVector4& color)
{
    ConeSegmentCount = segments;
    FCone cone;
    cone.ConeApex = start;
    cone.ConeBaseCenter = end;
    cone.ConeRadius = radius;
    cone.ConeHeight = (start-end).Magnitude();
    cone.Color = color;
    cone.ConeSegmentCount = ConeSegmentCount;
    Cones.Add(cone);
}

void UPrimitiveBatch::AddSphere(const FVector& center, float radius, const FVector4& color)
{
    FSphere Sphere;
    Sphere.Center = center;
    Sphere.Radius = radius;
    Sphere.Color = color;
    Spheres.Add(Sphere);
}

void UPrimitiveBatch::AddLine(const FVector& StartPos, const FVector& Direction, const float Length, const FVector4& Color)
{
    FLine Line;
    Line.Start = StartPos;
    Line.Direction = Direction;
    Line.Length = Length;
    Line.Color = Color;
    Lines.Add(Line);
}

