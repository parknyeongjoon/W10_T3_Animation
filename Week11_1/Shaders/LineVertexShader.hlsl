
cbuffer FMatrixBuffer : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
};

cbuffer FGridParametersData : register(b1)
{
    float GridSpacing;
    int GridCount; // 총 grid 라인 수
    float3 GridOrigin; // Grid의 중심
    float Padding;
};

cbuffer FPrimitiveCounts : register(b2)
{
    int BoundingBoxCount; // 렌더링할 AABB의 개수
    int SphereCount;
    int ConeCount; // 렌더링할 cone의 개수
    int LineCount;
    int CapsuleCount; 
    int OBBCount; 
    float pad1;
    float pad2;
};

struct FBoundingBoxData
{
    float3 bbMin;
    float padding0;
    float3 bbMax;
    float padding1;
};

struct FConeData
{
    float3 ConeApex; // 원뿔의 꼭짓점
    float ConeRadius; // 원뿔 밑면 반지름
    
    float3 ConeBaseCenter; // 원뿔 밑면 중심
    float ConeHeight; // 원뿔 높이 (Apex와 BaseCenter 간 차이)
    float4 Color;
    
    int ConeSegmentCount; // 원뿔 밑면 분할 수
    float pad[3];
};

struct FCapsuleData
{
    float3 Center;
    float Radius;
    float3 UpVector;
    float HalfHeight;
    float4 Color;
};

struct FOrientedBoxCornerData
{
    float3 corners[8]; // 회전/이동 된 월드 공간상의 8꼭짓점
};

struct FSphereData
{
    float3 Center; 
    float Radius; 
    float4 Color;
};

struct FLineData
{
    float3 Start;
    float Length;
    float3 Direction;
    float pad;
    float4 Color;
};

StructuredBuffer<FBoundingBoxData> g_BoundingBoxes : register(t3);
StructuredBuffer<FConeData> g_ConeData : register(t4);
StructuredBuffer<FOrientedBoxCornerData> g_OrientedBoxes : register(t5);
StructuredBuffer<FSphereData> g_SphereData : register(t6);
StructuredBuffer<FLineData> g_LineData : register(t7);
StructuredBuffer<FCapsuleData> g_Capsules : register(t8);

static const int BB_EdgeIndices[12][2] =
{
    { 0, 1 },
    { 1, 3 },
    { 3, 2 },
    { 2, 0 }, // 앞면
    { 4, 5 },
    { 5, 7 },
    { 7, 6 },
    { 6, 4 }, // 뒷면
    { 0, 4 },
    { 1, 5 },
    { 2, 6 },
    { 3, 7 } // 측면
};

struct VS_INPUT
{
    uint vertexID : SV_VertexID; // 0 또는 1: 각 라인의 시작과 끝
    uint instanceID : SV_InstanceID; // 인스턴스 ID로 grid, axis, bounding box를 구분
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

/////////////////////////////////////////////////////////////////////////
// Grid 위치 계산 함수
/////////////////////////////////////////////////////////////////////////
float3 ComputeGridPosition(uint instanceID, uint vertexID)
{
    int halfCount = GridCount / 2;
    float centerOffset = halfCount * 0.5; // grid 중심이 원점에 오도록

    float3 startPos;
    float3 endPos;
    
    if (instanceID < halfCount)
    {
        // 수직선: X 좌표 변화, Y는 -centerOffset ~ +centerOffset
        float x = GridOrigin.x + (instanceID - centerOffset) * GridSpacing;
        if (abs(x - GridOrigin.x) < 0.001)
        {
            startPos = float3(0, 0, 0);
            endPos = float3(0, (GridOrigin.y - centerOffset * GridSpacing), 0);
        }
        else
        {
            startPos = float3(x, GridOrigin.y - centerOffset * GridSpacing, GridOrigin.z);
            endPos = float3(x, GridOrigin.y + centerOffset * GridSpacing, GridOrigin.z);
        }
    }
    else
    {
        // 수평선: Y 좌표 변화, X는 -centerOffset ~ +centerOffset
        int idx = instanceID - halfCount;
        float y = GridOrigin.y + (idx - centerOffset) * GridSpacing;
        if (abs(y - GridOrigin.y) < 0.001)
        {
            startPos = float3(0, 0, 0);
            endPos = float3(-(GridOrigin.x + centerOffset * GridSpacing), 0, 0);
        }
        else
        {
            startPos = float3(GridOrigin.x - centerOffset * GridSpacing, y, GridOrigin.z);
            endPos = float3(GridOrigin.x + centerOffset * GridSpacing, y, GridOrigin.z);
        }

    }
    return (vertexID == 0) ? startPos : endPos;
}

/////////////////////////////////////////////////////////////////////////
// Axis 위치 계산 함수 (총 3개: X, Y, Z)
/////////////////////////////////////////////////////////////////////////
float3 ComputeAxisPosition(uint axisInstanceID, uint vertexID)
{
    float3 start = float3(0.0, 0.0, 0.0);
    float3 end;
    float zOffset = 0.f;
    if (axisInstanceID == 0)
    {
        // X 축: 빨간색
        end = float3(1000000.0, 0.0, zOffset);
    }
    else if (axisInstanceID == 1)
    {
        // Y 축: 초록색
        end = float3(0.0, 1000000.0, zOffset);
    }
    else if (axisInstanceID == 2)
    {
        // Z 축: 파란색
        end = float3(0.0, 0.0, 1000000.0 + zOffset);
    }
    else
    {
        end = start;
    }
    return (vertexID == 0) ? start : end;
}

/////////////////////////////////////////////////////////////////////////
// Bounding Box 위치 계산 함수
// bbInstanceID: StructuredBuffer에서 몇 번째 bounding box인지
// edgeIndex: 해당 bounding box의 12개 엣지 중 어느 것인지
/////////////////////////////////////////////////////////////////////////
float3 ComputeBoundingBoxPosition(uint bbInstanceID, uint edgeIndex, uint vertexID)
{
    FBoundingBoxData box = g_BoundingBoxes[bbInstanceID];
  
//    0: (bbMin.x, bbMin.y, bbMin.z)
//    1: (bbMax.x, bbMin.y, bbMin.z)
//    2: (bbMin.x, bbMax.y, bbMin.z)
//    3: (bbMax.x, bbMax.y, bbMin.z)
//    4: (bbMin.x, bbMin.y, bbMax.z)
//    5: (bbMax.x, bbMin.y, bbMax.z)
//    6: (bbMin.x, bbMax.y, bbMax.z)
//    7: (bbMax.x, bbMax.y, bbMax.z)
    int vertIndex = BB_EdgeIndices[edgeIndex][vertexID];
    float x = ((vertIndex & 1) == 0) ? box.bbMin.x : box.bbMax.x;
    float y = ((vertIndex & 2) == 0) ? box.bbMin.y : box.bbMax.y;
    float z = ((vertIndex & 4) == 0) ? box.bbMin.z : box.bbMax.z;
    return float3(x, y, z);
}

/////////////////////////////////////////////////////////////////////////
// Axis 위치 계산 함수 (총 3개: X, Y, Z)
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Cone 계산 함수
/////////////////////////////////////////////////
// Helper: 계산된 각도에 따른 밑면 꼭짓점 위치 계산

float3 ComputeConePosition(uint globalInstanceID, uint vertexID)
{
    // 모든 cone이 동일한 세그먼트 수를 가짐
    int N = g_ConeData[0].ConeSegmentCount;
    
    uint coneIndex = globalInstanceID / (2 * N);
    uint lineIndex = globalInstanceID % (2 * N);
    
    // cone 데이터 읽기
    FConeData cone = g_ConeData[coneIndex];
    
    // cone의 축 계산
    float3 axis = normalize(cone.ConeApex - cone.ConeBaseCenter);
    
    // axis에 수직인 두 벡터(u, v)를 생성
    float3 arbitrary = abs(dot(axis, float3(0, 0, 1))) < 0.99 ? float3(0, 0, 1) : float3(0, 1, 0);
    float3 u = normalize(cross(axis, arbitrary));
    float3 v = cross(axis, u);
    
    if (lineIndex < (uint) N)
    {
        // 측면 선분: cone의 꼭짓점과 밑면의 한 점을 잇는다.
        float angle = lineIndex * 6.28318530718 / N;
        float3 baseVertex = cone.ConeBaseCenter + (cos(angle) * u + sin(angle) * v) * cone.ConeRadius;
        return (vertexID == 0) ? cone.ConeApex : baseVertex;
    }
    else
    {
        // 밑면 둘레 선분: 밑면상의 인접한 두 점을 잇는다.
        uint idx = lineIndex - N;
        float angle0 = idx * 6.28318530718 / N;
        float angle1 = ((idx + 1) % N) * 6.28318530718 / N;
        float3 v0 = cone.ConeBaseCenter + (cos(angle0) * u + sin(angle0) * v) * cone.ConeRadius;
        float3 v1 = cone.ConeBaseCenter + (cos(angle1) * u + sin(angle1) * v) * cone.ConeRadius;
        return (vertexID == 0) ? v0 : v1;
    }
}
/////////////////////////////////////////////////////////////////////////
// OBB
/////////////////////////////////////////////////////////////////////////
float3 ComputeOrientedBoxPosition(uint obIndex, uint edgeIndex, uint vertexID)
{
    FOrientedBoxCornerData ob = g_OrientedBoxes[obIndex];
    int cornerID = BB_EdgeIndices[edgeIndex][vertexID];
    return ob.corners[cornerID];
}

/////////////////////////////////////////////////////////////////////////
// Sphere
/////////////////////////////////////////////////////////////////////////
float3 ComputeSpherePosition(uint globalInstanceID, uint vertexID)
{
    // 세 평면마다 N개의 선분 → 전체 인스턴스 수 = 3 * N
    int N = 32;

    uint ringIndex = globalInstanceID / N; // 0: XY, 1: YZ, 2: XZ
    uint segmentIndex = globalInstanceID % N; // 0 ~ N-1
    
    FSphereData sphere = g_SphereData[globalInstanceID / (3 * N)]; // 인스턴스 그룹 당 하나의 구 정보

    float radius = sphere.Radius;
    float3 center = sphere.Center;

    float angle0 = segmentIndex * 6.2831853 / N;
    float angle1 = ((segmentIndex + 1) % N) * 6.2831853 / N;

    float3 p0, p1;

    if (ringIndex == 0)
    {
        // XY 평면 원
        p0 = center + float3(cos(angle0), sin(angle0), 0) * radius;
        p1 = center + float3(cos(angle1), sin(angle1), 0) * radius;
    }
    else if (ringIndex == 1)
    {
        // YZ 평면 원
        p0 = center + float3(0, cos(angle0), sin(angle0)) * radius;
        p1 = center + float3(0, cos(angle1), sin(angle1)) * radius;
    }
    else
    {
        // XZ 평면 원
        p0 = center + float3(cos(angle0), 0, sin(angle0)) * radius;
        p1 = center + float3(cos(angle1), 0, sin(angle1)) * radius;
    }

    return (vertexID == 0) ? p0 : p1;
}

/////////////////////////////////////////////////////////////////////////
// Line
/////////////////////////////////////////////////////////////////////////
float3 ComputeLinePosition(uint globalInstanceID, uint vertexID)
{
    FLineData Line = g_LineData[globalInstanceID / 2];
    float3 start = Line.Start;
    float3 end = Line.Start + normalize(Line.Direction) * Line.Length;

    return (vertexID == 0) ? start : end;
}

/////////////////////////////////////////////////////////////////////////
// Capsule
/////////////////////////////////////////////////////////////////////////
float3 ComputeCapsulePosition(uint globalInstanceID, uint vertexID)
{
    const uint NumSegmentsCircle = 16; // 원 둘레 분할 수
    const uint NumVerticalLines = 4; // 세로 선 4개 (상하좌우)
    const uint NumHemisphereSegments = 8; // 반구 아치 세그먼트 수 (각 방향마다)

    const float PI = 3.14159265359f;

    const uint perCapsuleInstanceCount = (NumSegmentsCircle * 2) + NumVerticalLines + (NumHemisphereSegments * 4) * 2;
    // Top Hemisphere 4개 + Bottom Hemisphere 4개

    uint capsuleIndex = globalInstanceID / perCapsuleInstanceCount;
    uint localID = globalInstanceID % perCapsuleInstanceCount;

    FCapsuleData capsule = g_Capsules[capsuleIndex];

    float3 Up = normalize(capsule.UpVector);
    float3 arbitrary = (abs(Up.z) > 0.99f) ? float3(0, 1, 0) : float3(0, 0, 1);
    float3 Right = normalize(cross(Up, arbitrary));
    float3 Forward = normalize(cross(Right, Up));

    float3 topCenter = capsule.Center + Up * capsule.HalfHeight;
    float3 bottomCenter = capsule.Center - Up * capsule.HalfHeight;

    if (localID < NumSegmentsCircle)
    {
        // Top Circle
        float angle0 = (localID) * (2.0f * PI / NumSegmentsCircle);
        float angle1 = (localID + 1) * (2.0f * PI / NumSegmentsCircle);

        float3 p0 = topCenter + capsule.Radius * (cos(angle0) * Right + sin(angle0) * Forward);
        float3 p1 = topCenter + capsule.Radius * (cos(angle1) * Right + sin(angle1) * Forward);

        return (vertexID == 0) ? p0 : p1;
    }
    else if (localID < NumSegmentsCircle * 2)
    {
        // Bottom Circle
        uint idx = localID - NumSegmentsCircle;

        float angle0 = (idx) * (2.0f * PI / NumSegmentsCircle);
        float angle1 = (idx + 1) * (2.0f * PI / NumSegmentsCircle);

        float3 p0 = bottomCenter + capsule.Radius * (cos(angle0) * Right + sin(angle0) * Forward);
        float3 p1 = bottomCenter + capsule.Radius * (cos(angle1) * Right + sin(angle1) * Forward);

        return (vertexID == 0) ? p0 : p1;
    }
    else if (localID < NumSegmentsCircle * 2 + NumVerticalLines)
    {
        // Vertical Lines (4개)
        uint idx = localID - (NumSegmentsCircle * 2);

        float angle = (idx) * (PI * 0.5f); // 0°, 90°, 180°, 270°

        float3 offset = capsule.Radius * (cos(angle) * Right + sin(angle) * Forward);

        float3 p0 = topCenter + offset;
        float3 p1 = bottomCenter + offset;

        return (vertexID == 0) ? p0 : p1;
    }
    else if (localID < NumSegmentsCircle * 2 + NumVerticalLines + (NumHemisphereSegments * 4))
    {
        // Top Hemisphere
        uint idx = localID - (NumSegmentsCircle * 2 + NumVerticalLines);
        uint group = idx / NumHemisphereSegments; // 0: 0°, 1: 90°, 2: 180°, 3: 270°
        uint seg = idx % NumHemisphereSegments;

        float phi;
        if (group == 0)
            phi = 0.0f; // 0도
        else if (group == 1)
            phi = PI * 0.5f; // 90도
        else if (group == 2)
            phi = PI; // 180도
        else
            phi = PI * 1.5f; // 270도

        float theta0 = (seg) * (PI * 0.5f / NumHemisphereSegments);
        float theta1 = (seg + 1) * (PI * 0.5f / NumHemisphereSegments);

        float3 p0 = topCenter
            + capsule.Radius * (cos(theta0) * (cos(phi) * Right + sin(phi) * Forward) + sin(theta0) * Up);
        float3 p1 = topCenter
            + capsule.Radius * (cos(theta1) * (cos(phi) * Right + sin(phi) * Forward) + sin(theta1) * Up);

        return (vertexID == 0) ? p0 : p1;
    }
    else
    {
        // Bottom Hemisphere
        uint idx = localID - (NumSegmentsCircle * 2 + NumVerticalLines + (NumHemisphereSegments * 4));
        uint group = idx / NumHemisphereSegments; // 0: 0°, 1: 90°, 2: 180°, 3: 270°
        uint seg = idx % NumHemisphereSegments;

        float phi;
        if (group == 0)
            phi = 0.0f;
        else if (group == 1)
            phi = PI * 0.5f;
        else if (group == 2)
            phi = PI;
        else
            phi = PI * 1.5f;

        float theta0 = (seg) * (PI * 0.5f / NumHemisphereSegments);
        float theta1 = (seg + 1) * (PI * 0.5f / NumHemisphereSegments);

        float3 p0 = bottomCenter
            + capsule.Radius * (cos(theta0) * (cos(phi) * Right + sin(phi) * Forward) - sin(theta0) * Up);
        float3 p1 = bottomCenter
            + capsule.Radius * (cos(theta1) * (cos(phi) * Right + sin(phi) * Forward) - sin(theta1) * Up);

        return (vertexID == 0) ? p0 : p1;
    }
}

/////////////////////////////////////////////////////////////////////////
// 메인 버텍스 셰이더
/////////////////////////////////////////////////////////////////////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    float3 pos;
    float4 color;
    
    // Cone 하나당 (2 * SegmentCount) 선분.
    // ConeCount 개수만큼이므로 총 (2 * SegmentCount * ConeCount).
    uint coneInstCnt = ConeCount * 2 * g_ConeData[0].ConeSegmentCount;
    
    uint sphereInstCnt = SphereCount * 3 * 32;
    
    uint lineInstCnt = LineCount * 2;

    // Grid / Axis / AABB 인스턴스 개수 계산
    uint gridLineCount = GridCount; // 그리드 라인
    uint axisCount = 3; // X, Y, Z 축 (월드 좌표축)
    uint aabbInstanceCount = 12 * BoundingBoxCount; // AABB 하나당 12개 엣지

    // 1) "콘 인스턴스 시작" 지점
    uint coneInstanceStart = gridLineCount + axisCount + aabbInstanceCount;
    // 2) 구 구간 시작 지점
    uint sphereInstanceStart = coneInstanceStart + coneInstCnt;
    // 3) 선 구간 시작 지점
    uint lineInstanceStart = sphereInstanceStart + sphereInstCnt;
    // 4) 그 다음(=선 구간의 끝)이 곧 OBB 시작 지점
    uint obbInstanceStart = lineInstanceStart + lineInstCnt;
    uint obbInstanceCount = 12 * OBBCount; // OBB도 12 edges per box
    
    uint capsuleInstanceStart = obbInstanceStart + obbInstanceCount;
    uint capsuleInstanceCount = CapsuleCount * (16 * 2 + 4 + 16*4);
    
    // 이제 instanceID를 기준으로 분기
    if (input.instanceID < gridLineCount)
    {
        // 0 ~ (GridCount-1): 그리드
        pos = ComputeGridPosition(input.instanceID, input.vertexID);
        color = float4(0.1, 0.1, 0.1, 1.0);
    }
    else if (input.instanceID < gridLineCount + axisCount)
    {
        // 그 다음 (axisCount)개: 축(Axis)
        uint axisInstanceID = input.instanceID - gridLineCount;
        pos = ComputeAxisPosition(axisInstanceID, input.vertexID);

        // 축마다 색상
        if (axisInstanceID == 0)
            color = float4(1.0, 0.0, 0.0, 1.0); // X: 빨강
        else if (axisInstanceID == 1)
            color = float4(0.0, 1.0, 0.0, 1.0); // Y: 초록
        else
            color = float4(0.0, 0.0, 1.0, 1.0); // Z: 파랑
    }
    else if (input.instanceID < gridLineCount + axisCount + aabbInstanceCount)
    {
        // 그 다음 AABB 인스턴스 구간
        uint index = input.instanceID - (gridLineCount + axisCount);
        uint bbInstanceID = index / 12; // 12개가 1박스
        uint bbEdgeIndex = index % 12;
        
        pos = ComputeBoundingBoxPosition(bbInstanceID, bbEdgeIndex, input.vertexID);
        color = float4(1.0, 1.0, 0.0, 1.0); // 노란색
    }
    else if (input.instanceID < sphereInstanceStart)
    {
        // 그 다음 콘(Cone) 구간
        uint coneInstanceID = input.instanceID - coneInstanceStart;
        pos = ComputeConePosition(coneInstanceID, input.vertexID);
        int N = g_ConeData[0].ConeSegmentCount;
        uint coneIndex = coneInstanceID / (2 * N);
        
        color = g_ConeData[coneIndex].Color;
    }
    else if (input.instanceID < lineInstanceStart) // SphereCollider
    {
        //그 다음 sphere 구간
        uint sphereInstanceID = input.instanceID - sphereInstanceStart;
        pos = ComputeSpherePosition(sphereInstanceID, input.vertexID);
        int N = 32;
        uint sphereIndex = sphereInstanceID / (3 * N);
        
        color = float4(0.0, 1.0, 0.0, 1.0);
    }
    else if (input.instanceID < obbInstanceStart)
    {
        // 그 다음 line 구간
        uint lineInstanceID = input.instanceID - lineInstanceStart;
        pos = ComputeLinePosition(lineInstanceID, input.vertexID);
        uint lineIndex = lineInstanceID / 2;
        color = g_LineData[lineIndex].Color;
    }
    else if (input.instanceID < obbInstanceStart + obbInstanceCount) // BoxCollider
    {
        uint obbLocalID = input.instanceID - obbInstanceStart;
        uint obbIndex = obbLocalID / 12;
        uint edgeIndex = obbLocalID % 12;

        pos = ComputeOrientedBoxPosition(obbIndex, edgeIndex, input.vertexID);
        color = float4(0.0, 1.0, 0.0, 1.0);
    }
    else if (input.instanceID < capsuleInstanceStart + capsuleInstanceCount) // CapsuleCollider
    {
        uint capsuleLocalID = input.instanceID - capsuleInstanceStart;
        pos = ComputeCapsulePosition(capsuleLocalID, input.vertexID);
        uint capsuleIndex = capsuleLocalID / 100;
        
        color = float4(0.0, 1.0, 0.0, 1.0);
    }
    else
    {
        
    }

    // 출력 변환
    output.Position = mul(float4(pos, 1.0), Model);
    output.Position = mul(output.Position, ViewProj);
    output.Color = color;
    return output;
}