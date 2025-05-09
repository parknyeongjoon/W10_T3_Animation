#include "TestFBXLoader.h"

#include "Components/Material/Material.h"
#include "Engine/FLoaderOBJ.h"

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
    FbxImporter* Importer = FbxImporter::Create(FbxManager, "myImporter");
    FbxScene* Scene = FbxScene::Create(FbxManager, "myScene");
    
    bool bResult = Importer->Initialize(GetData(FilePath), -1, FbxManager->GetIOSettings());
    if (!bResult)
        return nullptr;

    FbxAxisSystem UnrealAxisSystem(
    FbxAxisSystem::eZAxis,
    FbxAxisSystem::eParityEven, // TODO Check
    FbxAxisSystem::eLeftHanded);
    
    UnrealAxisSystem.ConvertScene(Scene);
    
    Importer->Import(Scene);
    Importer->Destroy();

    FSkeletalMeshRenderData* NewMeshData = new FSkeletalMeshRenderData();
    FRefSkeletal* RefSkeletal = new FRefSkeletal();
    
    NewMeshData->Name = FilePath;
    RefSkeletal->Name = FilePath;
    
    ExtractFBXMeshData(Scene, NewMeshData, RefSkeletal);

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

    TArray<FbxNode*> BoneNodes;
    std::function<void(FbxNode*)> FindBones = [&BoneNodes, &FindBones](FbxNode* node)
    {
        if (!node)
            return;

        FbxNodeAttribute* attr = node->GetNodeAttribute();
        if (attr && attr->GetAttributeType() != FbxNodeAttribute::eSkeleton)
            BoneNodes.Add(node);

        for (int i = 0; i < node->GetChildCount(); ++i)
            FindBones(node->GetChild(i));
    };
    
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
}

/* Extract할 때 FBX의 Mapping Mode와 Reference Mode에 따라 모두 다르게 파싱을 진행해야 함!! */
void TestFBXLoader::ExtractMeshFromNode(FbxNode* Node, FSkeletalMeshRenderData* MeshData, FRefSkeletal* RefSkeletal)
{
    FbxMesh* Mesh = Node->GetMesh();
    if (Mesh)
    {
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
        
        // 인덱스 데이터 추출, 
        ExtractIndices(Mesh, MeshData, BaseVertexIndex);
        
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
    FRefSkeletal* RefSkeletal)
{

    // 1) Normal Element의 매핑 모드로 분기
    auto* NormalElem = Mesh->GetElementNormal();
    auto  mapMode    = NormalElem
                      ? NormalElem->GetMappingMode()
                      : FbxGeometryElement::eByControlPoint;

    int BaseVertexIndex = MeshData->Vertices.Num();

    // 2) 모드별로 정점 생성
    if (mapMode == FbxGeometryElement::eByControlPoint)
    {
        // ControlPoint 기준 1:1
        int CPCount = Mesh->GetControlPointsCount();
        MeshData->Vertices.Reserve(BaseVertexIndex + CPCount);

        for (int cpIdx = 0; cpIdx < CPCount; ++cpIdx)
            AddVertexFromControlPoint(Mesh, MeshData, cpIdx);
    }
    if (mapMode == FbxGeometryElement::eByPolygonVertex)
    {
        // 폴리곤×버텍스 수만큼 복제
        int totalPV = 0;
        int polyCnt = Mesh->GetPolygonCount();
        for (int p = 0; p < polyCnt; ++p)
            totalPV += Mesh->GetPolygonSize(p);

        MeshData->Vertices.Reserve(BaseVertexIndex + totalPV);

        for (int p = 0; p < polyCnt; ++p)
        {
            int polySize = Mesh->GetPolygonSize(p);
            for (int v = 0; v < polySize; ++v)
            {
                int cpIdx = Mesh->GetPolygonVertex(p, v);
                AddVertexFromControlPoint(Mesh, MeshData, cpIdx);
            }
        }
    }

    // 3) Attribute(법선, UV, 탄젠트, 스키닝) 채우기
    ExtractNormals       (Mesh, MeshData, BaseVertexIndex);
    ExtractUVs           (Mesh, MeshData, BaseVertexIndex);
    ExtractTangents      (Mesh, MeshData, BaseVertexIndex);
    ExtractSkinningData  (Mesh, MeshData, RefSkeletal, BaseVertexIndex);
}

void TestFBXLoader::ExtractNormals(
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* RenderData,
    int BaseVertexIndex)
{
    auto* NormalElem = Mesh->GetElementNormal();
    if (!NormalElem) return;

    // 매핑·레퍼런스 모드
    auto mapMode = NormalElem->GetMappingMode();
    auto refMode = NormalElem->GetReferenceMode();

    // 순회 인덱스
    int polyVertCounter   = 0;
    int vertexBufferIndex = BaseVertexIndex;

    // 인덱스 계산 람다
    std::function<int(int,int)> getIdx;
    if (mapMode == FbxGeometryElement::eByControlPoint) {
        if (refMode == FbxGeometryElement::eDirect)
            getIdx = [&](int cp, int pv){ return cp; };
        else
            getIdx = [&](int cp, int pv){ return NormalElem->GetIndexArray().GetAt(cp); };
    }
    else if (mapMode == FbxGeometryElement::eByPolygonVertex) {
        if (refMode == FbxGeometryElement::eDirect)
            getIdx = [&](int cp, int pv){ return pv; };
        else
            getIdx = [&](int cp, int pv){ return NormalElem->GetIndexArray().GetAt(pv); };
    }
    else {
        // eByPolygon, eAllSame 등 추가 지원 필요 시 여기
        return;
    }



    // 폴리곤-버텍스 순회
    int polyCount = Mesh->GetPolygonCount();

    for (int p = 0; p < polyCount; ++p)
    {
        int polySize = Mesh->GetPolygonSize(p);
        for (int v = 0; v < polySize; ++v)
        {
            int ctrlIdx = Mesh->GetPolygonVertex(p, v);
            int idx     = getIdx(ctrlIdx, polyVertCounter);

            auto Nor = NormalElem->GetDirectArray().GetAt(idx);
            auto& V   = RenderData->Vertices[vertexBufferIndex];
            V.Normal.X = (float)Nor[0];
            V.Normal.Y = (float)Nor[1];
            V.Normal.Z = (float)Nor[2];

            ++polyVertCounter;
            ++vertexBufferIndex;
        }
    }
}

void TestFBXLoader::ExtractUVs(
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* MeshData,
    int BaseVertexIndex)
{
    auto* UVElem = Mesh->GetElementUV(0);
    if (!UVElem) return;

    auto mapMode = UVElem->GetMappingMode();
    auto refMode = UVElem->GetReferenceMode();

    int polyVertCounter   = 0;
    int vertexBufferIndex = BaseVertexIndex;

    std::function<int(int,int)> getIdx;
    if (mapMode == FbxGeometryElement::eByControlPoint) {
        if (refMode == FbxGeometryElement::eDirect)
            getIdx = [&](int cp, int pv){ return cp; };
        else
            getIdx = [&](int cp, int pv){ return UVElem->GetIndexArray().GetAt(cp); };
    }
    else if (mapMode == FbxGeometryElement::eByPolygonVertex) {
        if (refMode == FbxGeometryElement::eDirect)
            getIdx = [&](int cp, int pv){ return pv; };
        else
            getIdx = [&](int cp, int pv){ return UVElem->GetIndexArray().GetAt(pv); };
    }
    else {
        return;
    }

    int polyCount = Mesh->GetPolygonCount();
    for (int p = 0; p < polyCount; ++p)
    {
        int polySize = Mesh->GetPolygonSize(p);
        for (int v = 0; v < polySize; ++v)
        {
            int ctrlIdx = Mesh->GetPolygonVertex(p, v);
            int idx     = getIdx(ctrlIdx, polyVertCounter);

            auto UV = UVElem->GetDirectArray().GetAt(idx);
            auto& V = MeshData->Vertices[vertexBufferIndex];
            V.TexCoord.X = (float)UV[0];
            V.TexCoord.Y = 1.0f - (float)UV[1];  // DirectX 좌표계 보정

            ++polyVertCounter;
            ++vertexBufferIndex;
        }
    }
}

void TestFBXLoader::ExtractTangents(
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* MeshData,
    int BaseVertexIndex)
{
    // Tangent 없으면 생성
    auto* TanElem = Mesh->GetElementTangent(0);
    if (!TanElem || TanElem->GetDirectArray().GetCount() == 0)
    {
        printf("FBX Tangent Empty!");
        Mesh->GenerateTangentsData(0, /*overwrite=*/ true);
        TanElem = Mesh->GetElementTangent(0);
        if (!TanElem) return;
    }

    auto mapMode = TanElem->GetMappingMode();
    auto refMode = TanElem->GetReferenceMode();

    int polyVertCounter   = 0;
    int vertexBufferIndex = BaseVertexIndex;

    std::function<int(int,int)> getIdx;
    if (mapMode == FbxGeometryElement::eByControlPoint) {
        if (refMode == FbxGeometryElement::eDirect)
            getIdx = [&](int cp, int pv){ return cp; };
        else
            getIdx = [&](int cp, int pv){ return TanElem->GetIndexArray().GetAt(cp); };
    }
    else if (mapMode == FbxGeometryElement::eByPolygonVertex) {
        if (refMode == FbxGeometryElement::eDirect)
            getIdx = [&](int cp, int pv){ return pv; };
        else
            getIdx = [&](int cp, int pv){ return TanElem->GetIndexArray().GetAt(pv); };
    }
    else {
        return;
    }

    int polyCount = Mesh->GetPolygonCount();
    for (int p = 0; p < polyCount; ++p)
    {
        int polySize = Mesh->GetPolygonSize(p);
        for (int v = 0; v < polySize; ++v)
        {
            int ctrlIdx = Mesh->GetPolygonVertex(p, v);
            int idx     = getIdx(ctrlIdx, polyVertCounter);

            auto Tan = TanElem->GetDirectArray().GetAt(idx);
            auto& V   = MeshData->Vertices[vertexBufferIndex];
            V.Tangent.X = (float)Tan[0];
            V.Tangent.Y = (float)Tan[1];
            V.Tangent.Z = (float)Tan[2];

            ++polyVertCounter;
            ++vertexBufferIndex;
        }
    }
}

void TestFBXLoader::ExtractSkinningData(
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* MeshData,
    FRefSkeletal* RefSkeletal,
    int BaseVertexIndex)
{
    // 1) 매핑 모드 재확인
    auto* NormalElem = Mesh->GetElementNormal();
    auto  mapMode    = NormalElem
                      ? NormalElem->GetMappingMode()
                      : FbxGeometryElement::eByControlPoint;

    // 2) ControlPoint별로 최대 4개 본/웨이트 수집
    int cpCount = Mesh->GetControlPointsCount();
    struct BW { int Indices[4]; float Weights[4]; };
    std::vector<BW> cpWeights(cpCount);
    // 초기화
    for (int i = 0; i < cpCount; ++i)
        for (int j = 0; j < 4; ++j)
            cpWeights[i].Indices[j] = 0,
            cpWeights[i].Weights[j] = 0.f;

    // 스킨(Deformer) 순회
    int skinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int s = 0; s < skinCount; ++s)
    {
        auto* skin = static_cast<FbxSkin*>(Mesh->GetDeformer(s, FbxDeformer::eSkin));
        if (!skin) continue;

        // 클러스터(본) 순회
        int clusterCount = skin->GetClusterCount();
        for (int c = 0; c < clusterCount; ++c)
        {
            auto* clus = skin->GetCluster(c);
            FbxNode* linkNode = clus->GetLink();
            if (!linkNode) continue;

            // ★ 하드코딩: Map에서 본 이름으로 인덱스 꺼내기
            FString boneName(linkNode->GetName());
            int*   pBoneIdx = RefSkeletal->BoneNameToIndexMap.Find(boneName);
            if (!pBoneIdx)
                continue;   // 레퍼런스 스켈레톤에 본이 없으면 스킵
            int boneIdx = *pBoneIdx;

            // ControlPoint 웨이트 애셋
            int*    cpIdxArr = clus->GetControlPointIndices();
            double* cpWArr   = clus->GetControlPointWeights();
            int     cnt      = clus->GetControlPointIndicesCount();

            for (int k = 0; k < cnt; ++k)
            {
                int   cpId = cpIdxArr[k];
                float w    = static_cast<float>(cpWArr[k]);
                auto& B    = cpWeights[cpId];

                // 가장 작은 슬롯 교체
                int minSlot = 0;
                for (int m = 1; m < 4; ++m)
                    if (B.Weights[m] < B.Weights[minSlot])
                        minSlot = m;
                if (w > B.Weights[minSlot])
                {
                    B.Indices[minSlot] = boneIdx;
                    B.Weights[minSlot] = w;
                }
            }
        }
    }

    // 3) ControlPoint별 정규화
    for (auto& B : cpWeights)
    {
        float sum = B.Weights[0] + B.Weights[1] + B.Weights[2] + B.Weights[3];
        if (sum > 0.f)
            for (int j = 0; j < 4; ++j)
                B.Weights[j] /= sum;
        else
            B.Indices[0] = 0, B.Weights[0] = 1.f;
    }

    // 4) 매핑 모드별로 복제된 Vertices에 웨이트 복사
    if (mapMode == FbxGeometryElement::eByControlPoint)
    {
        // ControlPoint 기준 1:1
        for (int cpIdx = 0; cpIdx < cpCount; ++cpIdx)
        {
            auto& Bsrc = cpWeights[cpIdx];
            auto& V    = MeshData->Vertices[BaseVertexIndex + cpIdx];
            for (int j = 0; j < 4; ++j)
            {
                V.BoneIndices[j] = Bsrc.Indices[j];
                V.BoneWeights[j] = Bsrc.Weights[j];
            }
        }
    }
    else // eByPolygonVertex
    {
        int polyVertCounter   = 0;
        int vertexBufferIndex = BaseVertexIndex;
        int polyCount         = Mesh->GetPolygonCount();

        for (int p = 0; p < polyCount; ++p)
        {
            int sz = Mesh->GetPolygonSize(p);
            for (int v = 0; v < sz; ++v)
            {
                int cpIdx = Mesh->GetPolygonVertex(p, v);
                auto& Bsrc = cpWeights[cpIdx];
                auto& V    = MeshData->Vertices[vertexBufferIndex];
                for (int j = 0; j < 4; ++j)
                {
                    V.BoneIndices[j] = Bsrc.Indices[j];
                    V.BoneWeights[j] = Bsrc.Weights[j];
                }

                ++polyVertCounter;
                ++vertexBufferIndex;
            }
        }
    }
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

void TestFBXLoader::UpdateBoundingBox(FSkeletalMeshRenderData MeshData)
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

FSkeletalMeshRenderData* TestFBXLoader::GetSkeletalRenderData(FString FilePath)
{
    // TODO: 폴더에서 가져올 수 있으면 가져오기
    if (SkeletalMeshData.Contains(FilePath))
    {
        return SkeletalMeshData[FilePath];
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

FSkeletalMeshRenderData TestFBXLoader::GetCopiedSkeletalRenderData(FString FilePath)
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

void TestFBXLoader::AddVertexFromControlPoint(
    FbxMesh* Mesh,
    FSkeletalMeshRenderData* MeshData,
    int ControlPointIndex)
{
    auto* ControlPoints = Mesh->GetControlPoints();
    FSkeletalVertex Vertex;

    // 위치
    auto& CP = ControlPoints[ControlPointIndex];
    Vertex.Position.X = static_cast<float>(CP[0]);
    Vertex.Position.Y = static_cast<float>(CP[1]);
    Vertex.Position.Z = static_cast<float>(CP[2]);
    Vertex.Position.W = 1.0f;

    // 기본값
    Vertex.Normal   = FVector(0.0f, 0.0f, 1.0f);
    Vertex.TexCoord = FVector2D(0.0f, 0.0f);
    Vertex.Tangent  = FVector(1.0f, 0.0f, 0.0f);

    MeshData->Vertices.Add(Vertex);
}