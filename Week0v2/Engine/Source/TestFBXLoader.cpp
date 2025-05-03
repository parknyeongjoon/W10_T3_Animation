#include "TestFBXLoader.h"
#include "Components/Material/Material.h"
#include "Engine/FLoaderOBJ.h"

bool TestFBXLoader::InitFBXManager()
{
    if (bInitialized)
        return true;

    FbxManager = FbxManager::Create();

    FbxIOSettings* IOSetting = FbxIOSettings::Create(FbxManager, IOSROOT);
    FbxManager->SetIOSettings(IOSetting);

    bInitialized = true;

    return true;
}

bool TestFBXLoader::InitFBX(const FString& FilePath)
{
    FbxImporter* Importer = FbxImporter::Create(FbxManager, "myImporter");
    FbxScene* Scene = FbxScene::Create(FbxManager, "myScene");
    
    bool bResult = Importer->Initialize(GetData("Contents\\" + FilePath), -1, FbxManager->GetIOSettings());
    if (!bResult)
        return false;

    Importer->Import(Scene);
    Importer->Destroy();

    FSkeletalMeshRenderData* NewMeshData = new FSkeletalMeshRenderData();
    NewMeshData->Name = FilePath;
    ExtractFBXMeshData(Scene, NewMeshData);
    SkeletalMeshData.Add(FilePath, NewMeshData);

    Scene->Destroy();
    
    return true;
}

void TestFBXLoader::ExtractFBXMeshData(const FbxScene* Scene, FSkeletalMeshRenderData* MeshData) const
{
    FbxNode* RootNode = Scene->GetRootNode();
    if (RootNode == nullptr)
        return;

    ExtractMeshFromNode(RootNode, MeshData);
}

void TestFBXLoader::ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData) const
{
    FbxMesh* Mesh = Node->GetMesh();
    // Mesh가 있다면 데이터 저장
    if (Mesh)
    {
        // 버텍스 데이터 추출
        ExtractVertices(Mesh, MeshData);
        
        // 인덱스 데이터 추출
        ExtractIndices(Mesh, MeshData);
        
        // 머테리얼 데이터 추출
        ExtractMaterials(Node, Mesh, MeshData);
        
        // 바운딩 박스 업데이트
        UpdateBoundingBox(MeshData);
    }

    // 자식 노드들에 대해 재귀적으로 수행
    int childCount = Node->GetChildCount();
    for (int i = 0; i < childCount; i++) {
        ExtractMeshFromNode(Node->GetChild(i), MeshData);
    }
}

void TestFBXLoader::ExtractVertices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData) const
{
    int VertexCount = Mesh->GetControlPointsCount();
    FbxVector4* ControlPoints = Mesh->GetControlPoints();
    
    // 기존 정점 개수를 저장 (여러 메쉬가 있을 경우 인덱스 오프셋으로 사용)
    int BaseVertexIndex = MeshData->Vertices.Num();
    
    // 정점 위치 데이터 추출
    for (int i = 0; i < VertexCount; i++)
    {
        FSkeletalVertex Vertex;
        
        // 위치 설정
        Vertex.position.X = static_cast<float>(ControlPoints[i][0]);
        Vertex.position.Y = static_cast<float>(ControlPoints[i][1]);
        Vertex.position.Z = static_cast<float>(ControlPoints[i][2]);
        
        // 기본값으로 초기화
        Vertex.normal = FVector(0.0f, 0.0f, 1.0f);
        Vertex.texCoord = FVector2D(0.0f, 0.0f);
        Vertex.tangent = FVector(1.0f, 0.0f, 0.0f);
        
        MeshData->Vertices.Add(Vertex);
    }
    
    // 법선 데이터 추출
    ExtractNormals(Mesh, MeshData, BaseVertexIndex);
    
    // UV 데이터 추출
    ExtractUVs(Mesh, MeshData, BaseVertexIndex);
    
    // 탄젠트 데이터 추출
    ExtractTangents(Mesh, MeshData, BaseVertexIndex);
}

void TestFBXLoader::ExtractNormals(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex) const
{
    FbxGeometryElementNormal* NormalElement = Mesh->GetElementNormal();
    if (!NormalElement)
        return;
        
    FbxGeometryElement::EMappingMode MappingMode = NormalElement->GetMappingMode();
    FbxGeometryElement::EReferenceMode ReferenceMode = NormalElement->GetReferenceMode();
    
    int PolygonCount = Mesh->GetPolygonCount();
    
    // 폴리곤 별로 순회하며 법선 데이터 추출
    for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++)
    {
        int PolySize = Mesh->GetPolygonSize(PolyIndex);
        
        for (int VertexIndex = 0; VertexIndex < PolySize; VertexIndex++)
        {
            int ControlPointIndex = Mesh->GetPolygonVertex(PolyIndex, VertexIndex);
            
            // 매핑 모드에 따른 법선 인덱스 계산
            int NormalIndex = 0;
            
            if (MappingMode == FbxGeometryElement::eByControlPoint)
            {
                if (ReferenceMode == FbxGeometryElement::eDirect)
                    NormalIndex = ControlPointIndex;
                else if (ReferenceMode == FbxGeometryElement::eIndexToDirect)
                    NormalIndex = NormalElement->GetIndexArray().GetAt(ControlPointIndex);
            }
            else if (MappingMode == FbxGeometryElement::eByPolygonVertex)
            {
                int VertexId = PolyIndex * 3 + VertexIndex;
                
                if (ReferenceMode == FbxGeometryElement::eDirect)
                    NormalIndex = VertexId;
                else if (ReferenceMode == FbxGeometryElement::eIndexToDirect)
                    NormalIndex = NormalElement->GetIndexArray().GetAt(VertexId);
            }
            
            // 법선 데이터 가져오기
            FbxVector4 Normal = NormalElement->GetDirectArray().GetAt(NormalIndex);
            
            // 해당 정점의 법선 설정
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].normal.X = static_cast<float>(Normal[0]);
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].normal.Y = static_cast<float>(Normal[1]);
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].normal.Z = static_cast<float>(Normal[2]);
        }
    }
}

void TestFBXLoader::ExtractUVs(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex) const
{
    FbxGeometryElementUV* UVElement = Mesh->GetElementUV(0); // 첫 번째 UV 채널 사용
    if (!UVElement)
        return;
        
    FbxGeometryElement::EMappingMode MappingMode = UVElement->GetMappingMode();
    FbxGeometryElement::EReferenceMode ReferenceMode = UVElement->GetReferenceMode();
    
    int PolygonCount = Mesh->GetPolygonCount();
    
    for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++)
    {
        int PolySize = Mesh->GetPolygonSize(PolyIndex);
        
        for (int VertexIndex = 0; VertexIndex < PolySize; VertexIndex++)
        {
            int ControlPointIndex = Mesh->GetPolygonVertex(PolyIndex, VertexIndex);
            int UVIndex = Mesh->GetTextureUVIndex(PolyIndex, VertexIndex);
            
            // 매핑 모드에 따른 UV 인덱스 계산
            if (MappingMode == FbxGeometryElement::eByControlPoint)
            {
                if (ReferenceMode == FbxGeometryElement::eDirect)
                    UVIndex = ControlPointIndex;
                else if (ReferenceMode == FbxGeometryElement::eIndexToDirect)
                    UVIndex = UVElement->GetIndexArray().GetAt(ControlPointIndex);
            }
            else if (MappingMode == FbxGeometryElement::eByPolygonVertex)
            {
                if (ReferenceMode == FbxGeometryElement::eDirect || ReferenceMode == FbxGeometryElement::eIndexToDirect)
                {
                    // UVIndex는 이미 Mesh->GetTextureUVIndex에서 계산됨
                }
            }
            
            // UV 데이터 가져오기
            FbxVector2 UV = UVElement->GetDirectArray().GetAt(UVIndex);
            
            // 해당 정점의 UV 설정
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].texCoord.X = static_cast<float>(UV[0]);
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].texCoord.Y = 1.0f - static_cast<float>(UV[1]); // DirectX UV 좌표계로 변환
        }
    }
}

void TestFBXLoader::ExtractTangents(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData, int BaseVertexIndex) const
{
    FbxGeometryElementTangent* TangentElement = Mesh->GetElementTangent();
    if (!TangentElement)
        return;
        
    FbxGeometryElement::EMappingMode MappingMode = TangentElement->GetMappingMode();
    FbxGeometryElement::EReferenceMode ReferenceMode = TangentElement->GetReferenceMode();
    
    int PolygonCount = Mesh->GetPolygonCount();
    
    for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++)
    {
        int PolySize = Mesh->GetPolygonSize(PolyIndex);
        
        for (int VertexIndex = 0; VertexIndex < PolySize; VertexIndex++)
        {
            int ControlPointIndex = Mesh->GetPolygonVertex(PolyIndex, VertexIndex);
            
            // 매핑 모드에 따른 탄젠트 인덱스 계산
            int TangentIndex = 0;
            
            if (MappingMode == FbxGeometryElement::eByControlPoint)
            {
                if (ReferenceMode == FbxGeometryElement::eDirect)
                    TangentIndex = ControlPointIndex;
                else if (ReferenceMode == FbxGeometryElement::eIndexToDirect)
                    TangentIndex = TangentElement->GetIndexArray().GetAt(ControlPointIndex);
            }
            else if (MappingMode == FbxGeometryElement::eByPolygonVertex)
            {
                int VertexId = PolyIndex * 3 + VertexIndex;
                
                if (ReferenceMode == FbxGeometryElement::eDirect)
                    TangentIndex = VertexId;
                else if (ReferenceMode == FbxGeometryElement::eIndexToDirect)
                    TangentIndex = TangentElement->GetIndexArray().GetAt(VertexId);
            }
            
            // 탄젠트 데이터 가져오기
            FbxVector4 Tangent = TangentElement->GetDirectArray().GetAt(TangentIndex);
            
            // 해당 정점의 탄젠트 설정
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].tangent.X = static_cast<float>(Tangent[0]);
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].tangent.Y = static_cast<float>(Tangent[1]);
            MeshData->Vertices[BaseVertexIndex + ControlPointIndex].tangent.Z = static_cast<float>(Tangent[2]);
        }
    }
}

void TestFBXLoader::ExtractIndices(FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData) const
{
    int PolygonCount = Mesh->GetPolygonCount();
    int BaseVertexIndex = MeshData->Vertices.Num() - Mesh->GetControlPointsCount();
    
    // 폴리곤 별로 순회하며 인덱스 데이터 추출
    for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++)
    {
        int PolySize = Mesh->GetPolygonSize(PolyIndex);
        
        // 삼각형으로 분할 (폴리곤이 4각형 이상일 경우)
        for (int i = 2; i < PolySize; i++)
        {
            // 삼각형 구성 (0, i-1, i)
            int ControlPoint0 = Mesh->GetPolygonVertex(PolyIndex, 0);
            int ControlPoint1 = Mesh->GetPolygonVertex(PolyIndex, i - 1);
            int ControlPoint2 = Mesh->GetPolygonVertex(PolyIndex, i);
            
            // 정점 인덱스 추가 (BaseVertexIndex 오프셋 적용)
            MeshData->Indices.Add(BaseVertexIndex + ControlPoint0);
            MeshData->Indices.Add(BaseVertexIndex + ControlPoint1);
            MeshData->Indices.Add(BaseVertexIndex + ControlPoint2);
        }
    }
}

void TestFBXLoader::ExtractMaterials(FbxNode* Node, FbxMesh* Mesh, FSkeletalMeshRenderData* MeshData) const
{
    int MaterialCount = Node->GetMaterialCount();
    int BaseIndexOffset = MeshData->Indices.Num();
    int TotalTriangleCount = 0;
    for (int PolyIndex = 0; PolyIndex < Mesh->GetPolygonCount(); PolyIndex++) {
        int PolySize = Mesh->GetPolygonSize(PolyIndex);
        TotalTriangleCount += (PolySize - 2); // 폴리곤을 삼각형으로 분할한 개수
    }
    BaseIndexOffset -= TotalTriangleCount * 3; // 총 삼각형 개수 * 3
    
    // 재질 정보 추출
    for (int i = 0; i < MaterialCount; i++)
    {
        FbxSurfaceMaterial* FbxMaterial = Node->GetMaterial(i);
        if (!FbxMaterial)
            continue;
            
        // 재질 이름 가져오기
        FString MaterialName = FbxMaterial->GetName();
        
        // 엔진 재질 생성 또는 검색
        UMaterial* Material = FManagerOBJ::GetMaterial(MaterialName);
        
        // 재질 추가
        int MaterialIndex = MeshData->Materials.Add(Material);
        
        // 폴리곤을 순회하며 현재 재질에 해당하는 폴리곤 수 계산
        int PolygonCount = Mesh->GetPolygonCount();
        int TriangleCount = 0;
        
        for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++)
        {
            int MaterialIndexForPolygon = Mesh->GetElementMaterial()->GetIndexArray().GetAt(PolyIndex);
            
            if (MaterialIndexForPolygon == i)
            {
                int PolySize = Mesh->GetPolygonSize(PolyIndex);
                TriangleCount += (PolySize - 2); // 삼각형 개수 계산
            }
        }

        // 재질 서브셋 정보 구성
        FMaterialSubset Subset;
        Subset.MaterialIndex = MaterialIndex;
        Subset.IndexStart = BaseIndexOffset; // 시작 인덱스
        Subset.MaterialName = MaterialName;
        Subset.IndexCount = TriangleCount * 3; // 삼각형 당 인덱스 3개
        MeshData->MaterialSubsets.Add(Subset);

        BaseIndexOffset += TriangleCount * 3;
    }
    
    // 메시에 재질이 없는 경우 기본 재질 생성
    if (MaterialCount == 0)
    {
        UMaterial* DefaultMaterial = FManagerOBJ::GetDefaultMaterial();
        int MaterialIndex = MeshData->Materials.Add(DefaultMaterial);
        
        FMaterialSubset Subset;
        Subset.MaterialName = DefaultMaterial->GetName();
        Subset.MaterialIndex = MaterialIndex;
        Subset.IndexStart = BaseIndexOffset;
        Subset.IndexCount = MeshData->Indices.Num() - BaseIndexOffset;
        
        MeshData->MaterialSubsets.Add(Subset);
    }
}

void TestFBXLoader::UpdateBoundingBox(FSkeletalMeshRenderData* MeshData) const
{
    if (MeshData->Vertices.Num() == 0)
        return;
        
    // 초기값 설정
    FVector Min = MeshData->Vertices[0].position;
    FVector Max = MeshData->Vertices[0].position;
    
    // 모든 정점을 순회하며 최소/최대값 업데이트
    for (int i = 1; i < MeshData->Vertices.Num(); i++)
    {
        const FVector& Pos = MeshData->Vertices[i].position;
        
        // 최소값 갱신
        Min.X = FMath::Min(Min.X, Pos.X);
        Min.Y = FMath::Min(Min.Y, Pos.Y);
        Min.Z = FMath::Min(Min.Z, Pos.Z);
        
        // 최대값 갱신
        Max.X = FMath::Max(Max.X, Pos.X);
        Max.Y = FMath::Max(Max.Y, Pos.Y);
        Max.Z = FMath::Max(Max.Z, Pos.Z);
    }
    
    // 바운딩 박스 설정
    MeshData->BoundingBox.min = Min;
    MeshData->BoundingBox.max = Max;
    MeshData->BoundingBox.Center = (Min + Max) * 0.5f;
    MeshData->BoundingBox.Extents = (Max - Min) * 0.5f;
}

FSkeletalMeshRenderData* TestFBXLoader::GetSkeletalMesh(FString FilePath)
{
    if (SkeletalMeshData.Contains(FilePath))
    {
        return SkeletalMeshData[FilePath];
    }
    
    return nullptr;
}