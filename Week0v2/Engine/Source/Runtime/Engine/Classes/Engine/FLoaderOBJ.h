#pragma once
#include <fstream>
#include <queue>
#include <set>
#include <sstream>

#include "Define.h"
#include "EditorEngine.h"
#include "Container/Map.h"
#include "HAL/PlatformType.h"
#include "Serialization/Serializer.h"

class UStaticMesh;
struct FManagerOBJ;
struct FLoaderOBJ
{
    // Obj Parsing (*.obj to FObjInfo)
    static bool ParseOBJ(const FString& ObjFilePath, FObjInfo& OutObjInfo);
    
    // Material Parsing (*.obj to MaterialInfo)
    static bool ParseMaterial(FObjInfo& OutObjInfo, OBJ::FStaticMeshRenderData& OutFStaticMesh);
    
    // Convert the Raw data to Cooked data (FStaticMeshRenderData)
    static bool ConvertToStaticMesh(const FObjInfo& RawData, OBJ::FStaticMeshRenderData& OutStaticMesh);
    
    static bool CreateTextureFromFile(const FWString& Filename);

    static void ComputeBoundingBox(const TArray<FVertexSimple>& InVertices, FVector& OutMinVector, FVector& OutMaxVector);
};

struct FManagerOBJ
{
public:
   
    static OBJ::FStaticMeshRenderData* LoadObjStaticMeshAsset(const FString& PathFileName);
    static bool LoadStaticMeshFromBinary(const FWString& FilePath, OBJ::FStaticMeshRenderData& OutStaticMesh);

    static UMaterial* CreateMaterial(const FObjMaterialInfo& materialInfo);
    static TMap<FString, UMaterial*>& GetMaterials() { return materialMap; }
    static UMaterial* GetMaterial(const FString& name);
    static int GetMaterialNum() { return materialMap.Num(); }
    static UStaticMesh* CreateStaticMesh(const FString& filePath);
    static const TMap<FWString, UStaticMesh*>& GetStaticMeshes() { return staticMeshMap; }
    static UStaticMesh* GetStaticMesh(const FWString& name);
    static int GetStaticMeshNum() { return staticMeshMap.Num(); }

private:
    static void CombineMaterialIndex(OBJ::FStaticMeshRenderData& OutFStaticMesh);
    static bool SaveStaticMeshToBinary(const FWString& wstring, const OBJ::FStaticMeshRenderData& static_mesh_render_data);
    static TArray<FString> ExtractMTLLibraryNamesFromOBJ(const FString& ObjFilePath);
    static FString ConvertMTLRelativePathToCurrent(const FString& ObjFilePath, const FString& MTLFileName);
    
    inline static TMap<FString, OBJ::FStaticMeshRenderData*> ObjStaticMeshMap;
    inline static TMap<FWString, UStaticMesh*> staticMeshMap;
    inline static TMap<FString, UMaterial*> materialMap;
};

struct Quadric
{
    float data[10] = {0};

    void Add(const Quadric& q) {
        for (int i = 0; i < 10; i++) {
            data[i] += q.data[i];
        }
    }
};

// 엣지 축소 후보 구조체 (operator< 정의로 우선순위 큐에서 낮은 cost 우선)
struct IndexSet
{
    IndexSet(uint32 v, uint32 n, uint32 t) :verIndex(v), norIndex(n), texIndex(t) {}
    uint32 verIndex, norIndex, texIndex;
    bool operator<(const IndexSet& other) const
    {
        return verIndex < other.verIndex;
    }
};
struct EdgeCollapse
{
    IndexSet i1, i2;
    float cost;
    FVector newPos;

    bool operator<(const EdgeCollapse& other) const
    {
        return cost > other.cost; // 최소 힙: 낮은 cost 우선
    }
};



class QEMSimplifier
{
public:
    // targetVertexCount까지 단순화 (예제에서는 면 정보를 이용하여 인접 정점만 후보로 처리)
    static void Simplify(FObjInfo& obj, int targetVertexCount);

private:
    // 간단한 엣지 축소 비용 계산 함수
    // 실제 구현에서는 newPos^T * q * newPos 를 계산하는 방식이 더 정확하지만, 여기서는 q.data[9]를 사용합니다.
    static float ComputeCollapseCost(const Quadric& q1, const Quadric& q2, const FVector& newPos);
    static float ComputeCollapseCost(const Quadric& q1, const Quadric& q2, const FVector& newPos, const FVector2D& uv1, const FVector2D& uv2,
                                     const FVector& normal1, const FVector& normal2);
};