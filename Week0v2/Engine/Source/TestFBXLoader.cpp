#include "TestFBXLoader.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Components/Material/Material.h"
#include "Engine/FLoaderOBJ.h"
#include "Math/Rotator.h"

bool TestFBXLoader::InitFBXManager()
{
    FbxManager = FbxManager::Create();

    FbxIOSettings* IOSetting = FbxIOSettings::Create(FbxManager, IOSROOT);
    FbxManager->SetIOSettings(IOSetting);

    return true;
}

FSkeletalMeshRenderData* TestFBXLoader::ParseFBX(const FString& FilePath)
{
    static bool bInitialized = false;
    if (bInitialized == false)
    {
        InitFBXManager();
        bInitialized = true;
    }
    FbxScene* Scene = FbxScene::Create(FbxManager, "myScene");
    FbxImporter* Importer = FbxImporter::Create(FbxManager, "myImporter");
    
    bool bResult = Importer->Initialize(GetData(FilePath), -1, FbxManager->GetIOSettings());
    if (!bResult)
        return nullptr;
    
    Importer->Import(Scene);
    Importer->Destroy();

    FbxAxisSystem UnrealAxisSystem(
    FbxAxisSystem::eZAxis,
    FbxAxisSystem::eParityEven, // TODO Check
    FbxAxisSystem::eLeftHanded);
    if (Scene->GetGlobalSettings().GetAxisSystem() != UnrealAxisSystem)
        UnrealAxisSystem.DeepConvertScene(Scene);
    
    FbxSystemUnit SceneSystemUnit = Scene->GetGlobalSettings().GetSystemUnit();
    if( SceneSystemUnit.GetScaleFactor() != 1.0 )
    {
        FbxSystemUnit::cm.ConvertScene(Scene);
    }
    
    FSkeletalMeshRenderData* NewMeshData = new FSkeletalMeshRenderData();
    FRefSkeletal* RefSkeletal = new FRefSkeletal();
    
    NewMeshData->Name = FilePath;
    RefSkeletal->Name = FilePath;
    
    ExtractFBXMeshData(Scene, NewMeshData, RefSkeletal);
    ExtractFBXAnimData(Scene, FilePath);

    for (const auto Vertex: NewMeshData->Vertices)
    {
        FSkeletalVertex RawVertex;
        RawVertex = Vertex;
        RefSkeletal->RawVertices.Add(RawVertex);
    }

    for (const auto Bone : NewMeshData->Bones)
    {
        FBone RawBone;
        RawBone = Bone;
        RefSkeletal->RawBones.Add(RawBone);
    }
    
    SkeletalMeshData.Add(FilePath, NewMeshData);
    RefSkeletalData.Add(FilePath, RefSkeletal);

    Scene->Destroy();
    
    return NewMeshData;
}

void TestFBXLoader::ExtractFBXMeshData(const FbxScene* Scene, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal)
{
    FbxNode* RootNode = Scene->GetRootNode();
    if (RootNode == nullptr)
        return;

    ExtractBoneFromNode(RootNode, MeshData, RefSkeletal);
    ExtractMeshFromNode(RootNode, MeshData, RefSkeletal);
}

void TestFBXLoader::ExtractBoneFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal)
{
    // Clear existing bone tree data
    RefSkeletal->BoneTree.Empty();
    RefSkeletal->RootBoneIndices.Empty();
    RefSkeletal->BoneNameToIndexMap.Empty();

    // Before - collect all bone nodes 
    TArray<FbxNode*> BoneNodes;
    std::function<void(FbxNode*)> FindBones = [&BoneNodes, &FindBones](FbxNode* node)
    {
        if (!node)
            return;

        FbxNodeAttribute* attr = node->GetNodeAttribute();
        if (attr && attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
            BoneNodes.Add(node);

        for (int i = 0; i < node->GetChildCount(); ++i)
            FindBones(node->GetChild(i));
    };
    FindBones(Node);
    
    // First pass - collect all bone nodes from clusters and add to flat bone array
    for (int i = 0; i < BoneNodes.Num(); ++i)
    {
        FbxNode* BoneNode = BoneNodes[i];
        
        if (!BoneNode)
            continue;
            
        FString BoneName = BoneNode->GetName();
        
        // Check if this bone already exists
        int* ExistingBoneIndex = RefSkeletal->BoneNameToIndexMap.Find(BoneName);
        if (ExistingBoneIndex)
            continue;
            
        // Create new bone and add to array
        FBone NewBone;
        NewBone.BoneName = BoneName;
        NewBone.ParentIndex = -1; // Will be set in second pass
        
        // Get transform matrices
        FbxAMatrix GlobalTransform = BoneNode->EvaluateGlobalTransform();
        FbxAMatrix LocalTransform = BoneNode->EvaluateLocalTransform();
        
        // Store transforms
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                NewBone.GlobalTransform.M[i][j] = static_cast<float>(GlobalTransform.Get(i, j));
                NewBone.LocalTransform.M[i][j] = static_cast<float>(LocalTransform.Get(i, j));
            }
        }
        
        // Add bone to array and create mapping
        int BoneIndex = MeshData->Bones.Add(NewBone);
        RefSkeletal->BoneNameToIndexMap.Add(BoneName, BoneIndex);
        
        // Create corresponding bone tree node
        FBoneNode NewNode;
        NewNode.BoneName = BoneName;
        NewNode.BoneIndex = BoneIndex;
        RefSkeletal->BoneTree.Add(NewNode);
    }
    
    // Second pass - establish parent-child relationships
    for (int i = 0; i < BoneNodes.Num(); ++i)
    {
        FbxNode* BoneNode = BoneNodes[i];
        
        if (!BoneNode)
            continue;
            
        FString BoneName = BoneNode->GetName();
        FbxNode* ParentNode = BoneNode->GetParent();
        
        if (!RefSkeletal->BoneNameToIndexMap.Contains(BoneName))
            continue;
            
        int BoneIndex = RefSkeletal->BoneNameToIndexMap[BoneName];
        
        if (ParentNode)
        {
            FString ParentName = ParentNode->GetName();
            
            // If parent is also a bone, establish the relationship
            if (RefSkeletal->BoneNameToIndexMap.Contains(ParentName))
            {
                int ParentIndex = RefSkeletal->BoneNameToIndexMap[ParentName];
                
                // Update parent index in the bone
                MeshData->Bones[BoneIndex].ParentIndex = ParentIndex;
                
                // Add this bone as a child of the parent in the tree structure
                RefSkeletal->BoneTree[ParentIndex].ChildIndices.Add(BoneIndex);
            }
        }
    }

    // Find and Save Root bone nodes
    for (int i = 0; i < MeshData->Bones.Num(); ++i)
    {
        if (MeshData->Bones[i].ParentIndex == -1)
        {
            RefSkeletal->RootBoneIndices.Add(i);
        }
    }
}

/* Extract할 때 FBX의 Mapping Mode와 Reference Mode에 따라 모두 다르게 파싱을 진행해야 함!! */
void TestFBXLoader::ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal)
{
    FbxMesh* Mesh = Node->GetMesh();
    if (Mesh)
    {
        if (!IsTriangulated(Mesh))
        {
            FbxGeometryConverter Converter = FbxGeometryConverter(FbxManager);
            Converter.Triangulate(Mesh, true);
            Mesh = Node->GetMesh();
        }
        
        int BaseVertexIndex = MeshData->Vertices.Num();
        int BaseIndexOffset = MeshData->Indices.Num();
        
        for (int d = 0; d < Mesh->GetDeformerCount(FbxDeformer::eSkin); ++d)
        {
            auto* Skin = static_cast<FbxSkin*>(Mesh->GetDeformer(d, FbxDeformer::eSkin));
            if (Skin)
            {
                // BaseVertexIndex는 Vertex 추출 직후 오프셋을 위해 넘겨 줍니다.
                ProcessSkinning(Skin, MeshData, RefSkeletal, BaseVertexIndex);
                // 보통 메시당 하나의 Skin만 쓰므로 break 해도 무방합니다.
                break;
            }
        }

        // 버텍스 데이터 추출
        ExtractVertices(Mesh, MeshData, RefSkeletal);
        
        // 인덱스 데이터 추출.
        // 250510) ExtractVertices에서 같이 추출하도록 수정.
        // ExtractIndices(Mesh, MeshData, BaseVertexIndex);
        
        // 머테리얼 데이터 추출
        ExtractMaterials(Node, Mesh, MeshData, RefSkeletal, BaseIndexOffset);
        
        // 바운딩 박스 업데이트
        UpdateBoundingBox(*MeshData);
    }

    // 자식 노드들에 대해 재귀적으로 수행
    int childCount = Node->GetChildCount();
    for (int i = 0; i < childCount; i++) {
        ExtractMeshFromNode(Node->GetChild(i), MeshData, RefSkeletal);
    }
}

void TestFBXLoader::ExtractVertices(
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* MeshData,
    FRefSkeletal* RefSkeletal
)
{
    IndexMap.Empty();
    SkinWeightMap.Empty();
    ExtractSkinningData(Mesh, RefSkeletal);


    int polyCnt = Mesh->GetPolygonCount();
    for (int p = 0; p < polyCnt; ++p)
    {
        int polySize = Mesh->GetPolygonSize(p);
        for (int v = 0; v < polySize; ++v)
        {
            // int cpIdx = Mesh->GetPolygonVertex(p, v);
            FSkeletalVertex vertex = GetVertexFromControlPoint(Mesh, p, v);
            ExtractNormal(Mesh, vertex, p, v);
            ExtractUV(Mesh, vertex, p, v);
            ExtractTangent(Mesh, vertex, p, v);
            StoreWeights(Mesh, vertex, p, v);
            StoreVertex(vertex, MeshData);
        }
    }
}

FSkeletalVertex& TestFBXLoader::GetVertexFromControlPoint(
    FbxMesh* Mesh,
    int PolygonIndex,
    int VertexIndex
)
{
    auto* ControlPoints = Mesh->GetControlPoints();
    FSkeletalVertex Vertex;

    // 위치
    // auto& CP = ControlPoints[ControlPointIndex];
    int controlPointIndex = Mesh->GetPolygonVertex(PolygonIndex, VertexIndex);
    Vertex.Position.X = static_cast<float>(ControlPoints[controlPointIndex][0]);
    Vertex.Position.Y = static_cast<float>(ControlPoints[controlPointIndex][1]);
    Vertex.Position.Z = static_cast<float>(ControlPoints[controlPointIndex][2]);
    Vertex.Position.W = 1.0f;
    
    // 기본값
    Vertex.Normal   = FVector(0.0f, 0.0f, 1.0f);
    Vertex.TexCoord = FVector2D(0.0f, 0.0f);
    Vertex.Tangent  = FVector4(1.0f, 0.0f, 0.0f, 1.0f);

    return Vertex;
}

void TestFBXLoader::ExtractNormal(
    FbxMesh* Mesh,
    FSkeletalVertex& Vertex,
    int PolygonIndex,
    int VertexIndex
)
{
    FbxLayerElementNormal* NormalElem = Mesh->GetElementNormal();
    if (!NormalElem) return;

    // 매핑·레퍼런스 모드
    auto mapMode = NormalElem->GetMappingMode();
    auto refMode = NormalElem->GetReferenceMode();
    int index = -1;

    switch (mapMode)
    {
    case FbxLayerElement::eNone:
        break;
    case FbxLayerElement::eByControlPoint:
        index = Mesh->GetPolygonVertex(PolygonIndex, VertexIndex);
        break;
    case FbxLayerElement::eByPolygonVertex:
        index = PolygonIndex * 3 + VertexIndex;
        break;
    case FbxLayerElement::eByPolygon:
        index = PolygonIndex;
        break;
    case FbxLayerElement::eByEdge:
        break;
    case FbxLayerElement::eAllSame:
        break;
    }

    if (refMode != FbxLayerElement::eDirect)
        index = NormalElem->GetIndexArray().GetAt(index);
    
    auto Normal = NormalElem->GetDirectArray().GetAt(index);
    Vertex.Normal.X = Normal[0];
    Vertex.Normal.Y = Normal[1];
    Vertex.Normal.Z = Normal[2];

}

void TestFBXLoader::ExtractUV(
    FbxMesh* Mesh,
    FSkeletalVertex& Vertex,
    int PolygonIndex,
    int VertexIndex
)
{
    FbxLayerElementUV* UVElem = Mesh->GetElementUV(0);
    if (!UVElem) return;

    // 매핑·레퍼런스 모드
    auto mapMode = UVElem->GetMappingMode();
    auto refMode = UVElem->GetReferenceMode();
    int index = -1;

    switch (mapMode)
    {
    case FbxLayerElement::eNone:
        break;
    case FbxLayerElement::eByControlPoint:
        index = Mesh->GetPolygonVertex(PolygonIndex, VertexIndex);
        break;
    case FbxLayerElement::eByPolygonVertex:
        index = PolygonIndex * 3 + VertexIndex;
        break;
    case FbxLayerElement::eByPolygon:
        index = PolygonIndex;
        break;
    case FbxLayerElement::eByEdge:
        break;
    case FbxLayerElement::eAllSame:
        break;
    }

    if (refMode != FbxLayerElement::eDirect)
        index = UVElem->GetIndexArray().GetAt(index);

    auto UV = UVElem->GetDirectArray().GetAt(index);
    Vertex.TexCoord.X = UV[0];
    Vertex.TexCoord.Y = 1.0f - UV[1];  // DirectX 좌표계 보정

}

void TestFBXLoader::ExtractTangent(
    FbxMesh* Mesh,
    FSkeletalVertex& Vertex,
    int PolygonIndex,
    int VertexIndex
)
{
    auto* TanElem = Mesh->GetElementTangent(0);
    if (!TanElem || TanElem->GetDirectArray().GetCount() == 0)
    {
        Mesh->GenerateTangentsData(0, /*overwrite=*/ true);
        TanElem = Mesh->GetElementTangent(0);
        if (!TanElem) return;
    }

    auto mapMode = TanElem->GetMappingMode();
    auto refMode = TanElem->GetReferenceMode();
    int index = -1;

    switch (mapMode)
    {
    case FbxLayerElement::eNone:
        break;
    case FbxLayerElement::eByControlPoint:
        index = Mesh->GetPolygonVertex(PolygonIndex, VertexIndex);
        break;
    case FbxLayerElement::eByPolygonVertex:
        index = PolygonIndex * 3 + VertexIndex;
        break;
    case FbxLayerElement::eByPolygon:
        index = PolygonIndex;
        break;
    case FbxLayerElement::eByEdge:
        break;
    case FbxLayerElement::eAllSame:
        break;
    }

    if (refMode != FbxLayerElement::eDirect)
        index = TanElem->GetIndexArray().GetAt(index);
    
    auto Tan = TanElem->GetDirectArray().GetAt(index);
    Vertex.Tangent.X = Tan[0];
    Vertex.Tangent.Y = Tan[1];
    Vertex.Tangent.Z = Tan[2];
    Vertex.Tangent.W = Tan[3];
}

void TestFBXLoader::ExtractSkinningData(
    FbxMesh* Mesh,
    FRefSkeletal* RefSkeletal
)
{
    if (!Mesh) return;
    
    int skinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int s = 0; s < skinCount; ++s)
    {
        FbxSkin* skin = static_cast<FbxSkin*>(Mesh->GetDeformer(s, FbxDeformer::eSkin));
        int clustureCount = skin->GetClusterCount();
        for (int c = 0; c < clustureCount; ++c)
        {
            FbxCluster* cluster = skin->GetCluster(c);
            FbxNode* linkedBone = cluster->GetLink();
            if (!linkedBone)
                continue;

            FString boneName = linkedBone->GetName();
            int boneIndex = RefSkeletal->BoneNameToIndexMap[boneName];

            int* indices = cluster->GetControlPointIndices();
            double* weights = cluster->GetControlPointWeights();
            int count = cluster->GetControlPointIndicesCount();
            for (int i = 0; i < count; ++i)
            {
                int ctrlIdx = indices[i];
                float weight = static_cast<float>(weights[i]);
                if (!SkinWeightMap.Contains(ctrlIdx))
                    SkinWeightMap.Add(ctrlIdx, TArray<FBoneWeightInfo>());
                SkinWeightMap[ctrlIdx].Add({boneIndex, weight});
            }
            
        }
    } 
}

void TestFBXLoader::StoreWeights(
    FbxMesh* Mesh,
    FSkeletalVertex& Vertex,
    int PolygonIndex,
    int VertexIndex
)
{
    int ControlPointIndex = Mesh->GetPolygonVertex(PolygonIndex, VertexIndex);
    for (int i = 0; i < std::min(SkinWeightMap[ControlPointIndex].Num(), 4); ++i)
    {
        const FBoneWeightInfo& info = SkinWeightMap[ControlPointIndex][i];
        Vertex.BoneIndices[i] = info.BoneIndex;
        Vertex.BoneWeights[i] = info.BoneWeight;
    }
}

void TestFBXLoader::StoreVertex(FSkeletalVertex& vertex, FSkeletalMeshRenderData* MeshData)
{
    std::stringstream ss;
    ss << vertex.Position.X << "," << vertex.Position.Y << "," << vertex.Position.Z << ",";
    ss << vertex.Normal.X << "," << vertex.Normal.Y << "," << vertex.Normal.Z << ",";
    ss << vertex.TexCoord.X << "," << vertex.TexCoord.Y;
    FString key = ss.str();
    uint32 index;
    if (!IndexMap.Contains(key))
    {
        index = MeshData->Vertices.Num();
        MeshData->Vertices.Add(vertex);
        IndexMap[key] = index;
    } else
    {
        index = IndexMap[key];
    }
    MeshData->Indices.Add(index);
}

void TestFBXLoader::ProcessSkinning(FbxSkin* Skin, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal, int BaseVertexIndex)
{
    int ClusterCount = Skin->GetClusterCount();

    for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
    {
        FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);
        FbxNode* BoneNode = Cluster->GetLink();
        
        if (!BoneNode)
            continue;
            
        FString BoneName = BoneNode->GetName();
        int* ExistingBoneIndex = RefSkeletal->BoneNameToIndexMap.Find(BoneName);
        if (!ExistingBoneIndex)
            continue;
        FBone& bone = MeshData->Bones[*ExistingBoneIndex];
            
        // Get binding pose transformation
        // NewBone.InverseBindPoseMatrix = FMatrix::Inverse(NewBone.GlobalTransform);
        FbxAMatrix MeshTransform; // Mesh의 바인드 시점의 Global Transform
        Cluster->GetTransformMatrix(MeshTransform); // 메시의 변환 행렬 (기준점)
        FbxAMatrix LinkTransform; // Bone의 바인드 시점의 Global Transform
        Cluster->GetTransformLinkMatrix(LinkTransform); // 본의 바인드 포즈 행렬
        
        FbxAMatrix InverseBindMatrix = LinkTransform.Inverse() * MeshTransform;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                bone.InverseBindPoseMatrix.M[i][j] = static_cast<float>(InverseBindMatrix.Get(i, j));
            }
        }
        
        bone.SkinningMatrix = bone.InverseBindPoseMatrix * bone.GlobalTransform;
    }
    
    // Process vertex weights
    for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
    {
        FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);
        FbxNode* BoneNode = Cluster->GetLink();
        
        if (!BoneNode || !RefSkeletal->BoneNameToIndexMap.Contains(BoneNode->GetName()))
            continue;
        
        int BoneIndex = RefSkeletal->BoneNameToIndexMap[BoneNode->GetName()];
        
        // Get control point indices and weights
        int VertexCount = Cluster->GetControlPointIndicesCount();
        int* ControlPointIndices = Cluster->GetControlPointIndices();
        double* ControlPointWeights = Cluster->GetControlPointWeights();
        
        // Apply weights to vertices
        for (int i = 0; i < VertexCount; i++)
        {
            int VertexIndex = BaseVertexIndex + ControlPointIndices[i];
            float Weight = static_cast<float>(ControlPointWeights[i]);
            
            // Make sure vertex index is valid
            if (VertexIndex >= 0 && VertexIndex < MeshData->Vertices.Num())
            {
                // Find first empty weight slot
                for (int j = 0; j < 4; j++)
                {
                    if (MeshData->Vertices[VertexIndex].BoneWeights[j] == 0.0f)
                    {
                        MeshData->Vertices[VertexIndex].BoneIndices[j] = BoneIndex;
                        MeshData->Vertices[VertexIndex].BoneWeights[j] = Weight;
                        break;
                    }
                }
            }
        }
    }
}

void TestFBXLoader::ExtractIndices(
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* MeshData,
    int BaseVertexIndex)
{
    // 1) 정점 생성 때 쓴 매핑 모드 재확인
    auto* NormalElem = Mesh->GetElementNormal();
    auto  mapMode    = NormalElem
                      ? NormalElem->GetMappingMode()
                      : FbxGeometryElement::eByControlPoint;

    int polyVertCounter = 0;   // 폴리곤-버텍스 모드용 누적 카운터
    int polyCount       = Mesh->GetPolygonCount();

    for (int p = 0; p < polyCount; ++p)
    {
        int polySize = Mesh->GetPolygonSize(p);
        int pvStart  = polyVertCounter;  // 이 폴리곤의 시작 polyVert 인덱스

        // 삼각형 팬 트라이앵글
        for (int i = 2; i < polySize; ++i)
        {
            if (mapMode == FbxGeometryElement::eByControlPoint)
            {
                // ControlPoint 기준 인덱스
                int c0 = Mesh->GetPolygonVertex(p, 0);
                int c1 = Mesh->GetPolygonVertex(p, i - 1);
                int c2 = Mesh->GetPolygonVertex(p, i);
                MeshData->Indices.Add(BaseVertexIndex + c0);
                MeshData->Indices.Add(BaseVertexIndex + c1);
                MeshData->Indices.Add(BaseVertexIndex + c2);
            }
            else
            {
                // 폴리곤-버텍스 기준 인덱스
                MeshData->Indices.Add(BaseVertexIndex + pvStart + 0);
                MeshData->Indices.Add(BaseVertexIndex + pvStart + (i - 1));
                MeshData->Indices.Add(BaseVertexIndex + pvStart + i);
            }
        }

        polyVertCounter += polySize;
    }
}


void TestFBXLoader::ExtractMaterials(
    FbxNode* Node,
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* MeshData,
    FRefSkeletal* RefSkeletal,
    int BaseIndexOffset)
{
    auto* MatElem = Mesh->GetElementMaterial();
    int  matCount = Node->GetMaterialCount();

    // 매핑·레퍼런스 모드
    auto mapMode = MatElem
                 ? MatElem->GetMappingMode()
                 : FbxGeometryElement::eAllSame;
    auto refMode = MatElem
                 ? MatElem->GetReferenceMode()
                 : FbxGeometryElement::eDirect;

    int polyVertCounter = 0;
    int polyCount       = Mesh->GetPolygonCount();

    // 총 삼각형 수 미리 계산 (eAllSame, eByPolygon 모두 공통)
    int totalTris = 0;
    for (int p = 0; p < polyCount; ++p)
        totalTris += Mesh->GetPolygonSize(p) - 2;

    int currentOffset = BaseIndexOffset;

    for (int matIdx = 0; matIdx < matCount; ++matIdx)
    {
        // 이 재질에 속하는 삼각형 개수 세기
        int triCount = 0;
        polyVertCounter = 0;

        for (int p = 0; p < polyCount; ++p)
        {
            int polySize = Mesh->GetPolygonSize(p);

            // 폴리곤 하나당 매핑된 재질 인덱스를 구하는 방법
            int thisMat = 0;
            switch (mapMode)
            {
            case FbxGeometryElement::eAllSame:
                thisMat = 0; 
                break;
            case FbxGeometryElement::eByPolygon:
                thisMat = MatElem->GetIndexArray().GetAt(p);
                break;
            case FbxGeometryElement::eByPolygonVertex:
                {
                    // 1) 항상 IndexArray에서 머티리얼 레이어 인덱스 꺼내기
                    int layerMatIdx = MatElem->GetIndexArray().GetAt(polyVertCounter);

                    // 2) 그 인덱스로 노드의 실제 머티리얼 얻기
                    //    (thisMat은 단순히 머티리얼 번호로 사용)
                    thisMat = layerMatIdx;
                    break;
                }
            default:
                thisMat = 0;
            }

            if (thisMat == matIdx)
                triCount += (polySize - 2);

            polyVertCounter += (mapMode == FbxGeometryElement::eByPolygonVertex
                                ? polySize
                                : 1);
        }

        // Subset 만들기
        // Material 생성 & 등록
        FbxSurfaceMaterial* srcMtl = Node->GetMaterial(matIdx);
        FString            mtlName = srcMtl ? FString(srcMtl->GetName()) : TEXT("Mat") + FString::FromInt(matIdx);
        auto                newMtl = FManagerOBJ::CreateMaterial(ConvertFbxToObjMaterialInfo(srcMtl));
        int                 finalIdx = RefSkeletal->Materials.Add(newMtl);

        FMaterialSubset subset;
        subset.MaterialName  = mtlName;
        subset.MaterialIndex = finalIdx;
        subset.IndexStart    = currentOffset;
        subset.IndexCount    = triCount * 3;
        RefSkeletal->MaterialSubsets.Add(subset);

        currentOffset += triCount * 3;
    }

    // 재질이 하나도 없으면 디폴트
    if (matCount == 0)
    {
        UMaterial* DefaultMaterial = FManagerOBJ::GetDefaultMaterial();
        int MaterialIndex = RefSkeletal->Materials.Add(DefaultMaterial);
        
        FMaterialSubset Subset;
        Subset.MaterialName = DefaultMaterial->GetName();
        Subset.MaterialIndex = MaterialIndex;
        Subset.IndexStart = BaseIndexOffset;
        Subset.IndexCount = MeshData->Indices.Num() - BaseIndexOffset;
        
        RefSkeletal->MaterialSubsets.Add(Subset);
    }
}

void TestFBXLoader::UpdateBoundingBox(FSkeletalMeshRenderData& MeshData)
{
    if (MeshData.Vertices.Num() == 0)
        return;
        
    // 초기값 설정
    FVector Min = MeshData.Vertices[0].Position.xyz();
    FVector Max = MeshData.Vertices[0].Position.xyz();
    
    // 모든 정점을 순회하며 최소/최대값 업데이트
    for (int i = 1; i < MeshData.Vertices.Num(); i++)
    {
        const FVector& Pos = MeshData.Vertices[i].Position.xyz();
        
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
    MeshData.BoundingBox.min = Min;
    MeshData.BoundingBox.max = Max;
}

void TestFBXLoader::ExtractFBXAnimData(const FbxScene* scene, const FString& FilePath)
{
    int AnimStackCount = scene->GetSrcObjectCount<FbxAnimStack>();

    // Before - collect all bone nodes 
    TArray<FbxNode*> BoneNodes;
    std::function<void(FbxNode*)> FindBones = [&BoneNodes, &FindBones](FbxNode* node)
    {
        if (!node)
            return;

        FbxNodeAttribute* attr = node->GetNodeAttribute();
        if (attr && attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
            BoneNodes.Add(node);

        for (int i = 0; i < node->GetChildCount(); ++i)
            FindBones(node->GetChild(i));
    };
    FindBones(scene->GetRootNode());

    // parse AnimClips (AnimLayers)
    for (int i = 0; i < AnimStackCount; ++i)
    {
        FbxAnimStack* AnimStack = scene->GetSrcObject<FbxAnimStack>(i);
        if (!AnimStack)
            continue;

        ExtractAnimClip(AnimStack, BoneNodes, FilePath);
    }
}

void TestFBXLoader::ExtractAnimClip(FbxAnimStack* AnimStack, const TArray<FbxNode*>& BoneNodes, const FString& FilePath)
{
    UAnimDataModel* AnimData = FObjectFactory::ConstructObject<UAnimDataModel>(nullptr);
    AnimData->Name = AnimStack->GetName();
    
    FbxTime::EMode timeMode = AnimStack->GetScene()->GetGlobalSettings().GetTimeMode();
    switch (timeMode)
    {
    case FbxTime::eFrames120:
        AnimData->FrameRate = FFrameRate(120, 1); break;
    case FbxTime::eFrames100:
        AnimData->FrameRate = FFrameRate(100, 1); break;
    case FbxTime::eFrames60:
        AnimData->FrameRate = FFrameRate(60, 1); break;
    case FbxTime::eFrames50:
        AnimData->FrameRate = FFrameRate(50, 1); break;
    case FbxTime::eFrames48:
        AnimData->FrameRate = FFrameRate(48, 1); break;
    case FbxTime::eFrames30:
        AnimData->FrameRate = FFrameRate(30, 1); break;
    case FbxTime::eFrames24:
        AnimData->FrameRate = FFrameRate(24, 1); break;
    }

    AnimData->PlayLength = static_cast<float>(AnimStack->GetLocalTimeSpan().GetDuration().GetSecondDouble());
    AnimData->NumberOfFrames = FMath::Floor( AnimData->PlayLength / AnimData->FrameRate.AsInterval() + 0.5f );
    AnimData->NumberOfKeys = AnimData->NumberOfFrames + 1;
    
    int layerCount = AnimStack->GetMemberCount<FbxAnimLayer>();
    for (int i = 0; i < BoneNodes.Num(); ++i)
    {
        FbxNode* BoneNode = BoneNodes[i];
        if (!BoneNode)
            continue;
        FBoneAnimationTrack AnimTrack;
        AnimTrack.Name = BoneNode->GetName();
        
        FRawAnimSequenceTrack AnimTrackData;
        for (int j = 0; j < layerCount; ++j)
        {
            FbxAnimLayer* AnimLayer = AnimStack->GetMember<FbxAnimLayer>(j);
            if (!AnimLayer)
                continue;

            ExtractAnimTrack(BoneNode, AnimTrackData, AnimData);
            // ExtractAnimCurve(AnimLayer, BoneNode, AnimTrackData);
        }
        AnimTrack.InternalTrackData = AnimTrackData;

        AnimData->BoneAnimationTracks.Add(AnimTrack);
    }
    
    AnimDataMap.Add(FilePath + "\\" + AnimData->Name, AnimData);
}

void TestFBXLoader::ExtractAnimTrack(FbxNode* BoneNode, FRawAnimSequenceTrack& AnimTrack, const UAnimDataModel* AnimData)
{
    for (int i = 0; i < AnimData->NumberOfKeys; ++i)
    {
        FbxTime t;
        t.SetTime(0, 0, 0, i);
        FbxAMatrix tf = BoneNode->EvaluateLocalTransform(t);
        FbxVector4 translation = tf.GetT();
        FbxQuaternion rotation = tf.GetQ();
        FbxVector4 scaling = tf.GetS();
        
        AnimTrack.PosKeys.Add(FVector(
            translation[0],
            translation[1],
            translation[2]
        ));

        // quaternion 순서 맞추기 (xyzw -> wxyz)
        AnimTrack.RotKeys.Add(FQuat(
            rotation[3],
            rotation[0],
            rotation[1],
            rotation[2]
        ));

        AnimTrack.ScaleKeys.Add(FVector(
            scaling[0],
            scaling[1],
            scaling[2]
        ));
    }
}

void TestFBXLoader::ExtractAnimCurve(FbxAnimLayer* AnimLayer, FbxNode* BoneNode, FRawAnimSequenceTrack& AnimTrack)
{
    FbxAnimCurve* tx = BoneNode->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    FbxAnimCurve* ty = BoneNode->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    FbxAnimCurve* tz = BoneNode->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    FbxAnimCurve* rx = BoneNode->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    FbxAnimCurve* ry = BoneNode->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    FbxAnimCurve* rz = BoneNode->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    FbxAnimCurve* sx = BoneNode->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    FbxAnimCurve* sy = BoneNode->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    FbxAnimCurve* sz = BoneNode->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

    TArray<FbxTime> keyTimes;
    auto HasKeyTimes = [&keyTimes](FbxTime t) -> bool
    {
        for (const FbxTime& k : keyTimes)
        {
            if (k.GetSecondDouble() == t.GetSecondDouble())
                return true;
        }
        return false;
    };
    
    if (tx && ty && tz)
    {
        int keyCount = tx->KeyGetCount();
        for (int i = 0; i < keyCount; ++i)
        {
            FbxTime t = tx->KeyGetTime(i);
            if (!HasKeyTimes(t))
                keyTimes.Add(t);
        }
    }
    if (rx && ry && rz)
    {
        int keyCount = rx->KeyGetCount();
        for (int i = 0; i < keyCount; ++i)
        {
            FbxTime t = rx->KeyGetTime(i);
            if (!HasKeyTimes(t))
                keyTimes.Add(t);
        }
    }
    if (sx && sy && sz)
    {
        int keyCount = sx->KeyGetCount();
        for (int i = 0; i < keyCount; ++i)
        {
            FbxTime t = sx->KeyGetTime(i);
            if (!HasKeyTimes(t))
                keyTimes.Add(t);
        }
    }
    std::sort(keyTimes.begin(), keyTimes.end(), [](const FbxTime& a, const FbxTime& b)->bool { return a.GetSecondDouble() < b.GetSecondDouble(); });

    for (const FbxTime& time : keyTimes)
    {
        float t = time.GetSecondDouble();
        AnimTrack.KeyTimes.Add(t);
        if (tx && ty && tz)
        {
            AnimTrack.PosKeys.Add( FVector(
                tx->Evaluate(time),    
                ty->Evaluate(time),    
                tz->Evaluate(time)    
            ));
        }
        if (rx && ry && rz)
        {
            AnimTrack.RotKeys.Add( FRotator(
                rx->Evaluate(time),
                ry->Evaluate(time),
                rz->Evaluate(time)
            ).ToQuaternion());
        }
        if (sx && sy && sz)
        {
            AnimTrack.ScaleKeys.Add( FVector(
                sx->Evaluate(time),    
                sy->Evaluate(time),    
                sz->Evaluate(time)    
            ));
        }
    }

    // 원래는 key마다 보간 모드가 개별로 있는데
    // 현재 자료구조상 하드코딩으로 설정
    AnimTrack.InterpMode = EAnimInterpolationType::Cubic;
}

FSkeletalMeshRenderData* TestFBXLoader::GetSkeletalRenderData(const FString& FilePath)
{
    // TODO: 폴더에서 가져올 수 있으면 가져오기
    if (SkeletalMeshData.Contains(FilePath))
    {
        return SkeletalMeshData[FilePath];
    }
    
    return nullptr;
}

UAnimDataModel* TestFBXLoader::GetAnimData(const FString& FilePath)
{
    if (AnimDataMap.Contains(FilePath))
    {
        return AnimDataMap[FilePath];
    }
    
    return nullptr;
}

// FBX 머티리얼 → FObjMaterialInfo 변환 헬퍼
FObjMaterialInfo TestFBXLoader::ConvertFbxToObjMaterialInfo(
    FbxSurfaceMaterial* FbxMat,
    const FString& BasePath)
{
    FObjMaterialInfo OutInfo;

    // Material Name
    OutInfo.MTLName = FString(FbxMat->GetName());
    
    // Lambert 전용 프로퍼티
    if (auto* Lam = FbxCast<FbxSurfaceLambert>(FbxMat))
    {
        // Ambient
        {
            auto c = Lam->Ambient.Get();
            float f = (float)Lam->AmbientFactor.Get();
            OutInfo.Ambient = FVector(c[0]*f, c[1]*f, c[2]*f);
        }
        // Diffuse
        {
            auto c = Lam->Diffuse.Get();
            float f = (float)Lam->DiffuseFactor.Get();
            OutInfo.Diffuse = FVector(c[0]*f, c[1]*f, c[2]*f);
        }
        // Emissive
        {
            auto c = Lam->Emissive.Get();
            float f = (float)Lam->EmissiveFactor.Get();
            OutInfo.Emissive = FVector(c[0]*f, c[1]*f, c[2]*f);
        }
        // BumpScale
        OutInfo.NormalScale = (float)Lam->BumpFactor.Get();
    }

    // Phong 전용 프로퍼티
    if (auto* Pho = FbxCast<FbxSurfacePhong>(FbxMat))
    {
        // Specular
        {
            auto c = Pho->Specular.Get();
            OutInfo.Specular = FVector((float)c[0], (float)c[1], (float)c[2]);
        }
        // Shininess
        OutInfo.SpecularScalar = (float)Pho->Shininess.Get();
    }

    // 공통 프로퍼티
    {
        // TransparencyFactor
        if (auto prop = FbxMat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor); prop.IsValid())
        {
            double tf = prop.Get<FbxDouble>();
            OutInfo.TransparencyScalar = (float)tf;
            OutInfo.bTransparent = OutInfo.TransparencyScalar < 1.f - KINDA_SMALL_NUMBER;
        }

        // Index of Refraction
        constexpr char const* sIndexOfRefraction = "IndexOfRefraction";
        if (auto prop = FbxMat->FindProperty(sIndexOfRefraction); prop.IsValid())
        {
            OutInfo.DensityScalar = (float)prop.Get<FbxDouble>();
        }

        // Illumination Model은 FBX에 따로 없으므로 기본 0
        OutInfo.IlluminanceModel = 0;
    }

    // 텍스처 채널 (Diffuse, Ambient, Specular, Bump/Normal, Alpha)
    auto ReadFirstTexture = [&](const char* PropName, FString& OutName, FWString& OutPath)
    {
        auto prop = FbxMat->FindProperty(PropName);
        if (!prop.IsValid()) return;
        int nbTex = prop.GetSrcObjectCount<FbxFileTexture>();
        if (nbTex <= 0) return;
        if (auto* Tex = prop.GetSrcObject<FbxFileTexture>(0))
        {
            FString fname = FString(Tex->GetFileName());
            OutName = fname;
            OutPath = (BasePath + fname).ToWideString();
            OutInfo.bHasTexture = true;
        }
    };

    // map_Kd
    ReadFirstTexture(FbxSurfaceMaterial::sDiffuse,
                     OutInfo.DiffuseTextureName,
                     OutInfo.DiffuseTexturePath);
    // map_Ka
    ReadFirstTexture(FbxSurfaceMaterial::sAmbient,
                     OutInfo.AmbientTextureName,
                     OutInfo.AmbientTexturePath);
    // map_Ks
    ReadFirstTexture(FbxSurfaceMaterial::sSpecular,
                     OutInfo.SpecularTextureName,
                     OutInfo.SpecularTexturePath);
    // map_Bump 또는 map_Ns
    ReadFirstTexture(FbxSurfaceMaterial::sBump,
                     OutInfo.BumpTextureName,
                     OutInfo.BumpTexturePath);
    ReadFirstTexture(FbxSurfaceMaterial::sNormalMap,
                     OutInfo.NormalTextureName,
                     OutInfo.NormalTexturePath);
    // map_d (Alpha)
    ReadFirstTexture(FbxSurfaceMaterial::sTransparentColor,
                     OutInfo.AlphaTextureName,
                     OutInfo.AlphaTexturePath);

    return OutInfo;
}

USkeletalMesh* TestFBXLoader::CreateSkeletalMesh(const FString& FilePath)
{
    // 있으면 return
    USkeletalMesh* SkeletalMesh = GetSkeletalMesh(FilePath);
    if (SkeletalMesh != nullptr)
    {
        USkeletalMesh* NewSkeletalMesh = SkeletalMesh->Duplicate(nullptr);
        NewSkeletalMesh->SetData(FilePath);
        return NewSkeletalMesh;
    }
    
    FSkeletalMeshRenderData* MeshData = ParseFBX(FilePath);
    if (MeshData == nullptr)
        return nullptr;
    
    SkeletalMesh = FObjectFactory::ConstructObject<USkeletalMesh>(nullptr);
    SkeletalMesh->SetData(FilePath);
    
    SkeletalMeshMap.Add(FilePath, SkeletalMesh);
    return SkeletalMesh;
}

USkeletalMesh* TestFBXLoader::GetSkeletalMesh(const FString& FilePath)
{
    if (SkeletalMeshMap.Contains(FilePath))
        return SkeletalMeshMap[FilePath];

    return nullptr;
}

FSkeletalMeshRenderData TestFBXLoader::GetCopiedSkeletalRenderData(const FString& FilePath)
{
    // 있으면 가져오고
    FSkeletalMeshRenderData* OriginRenderData = SkeletalMeshData[FilePath];
    if (OriginRenderData != nullptr)
    {
        return *OriginRenderData;
    }
    // 없으면 기본 생성자
    return {};
}

FRefSkeletal* TestFBXLoader::GetRefSkeletal(FString FilePath)
{
    // TODO: 폴더에서 가져올 수 있으면 가져오기
    if (RefSkeletalData.Contains(FilePath))
    {
        return RefSkeletalData[FilePath];
    }
    
    return nullptr;
}

bool TestFBXLoader::IsTriangulated(FbxMesh* Mesh)
{
    for (int i = 0; i < Mesh->GetPolygonCount(); ++i)
    {
        if (Mesh->GetPolygonSize(i) != 3)
            return false;
    }
    return true;
}