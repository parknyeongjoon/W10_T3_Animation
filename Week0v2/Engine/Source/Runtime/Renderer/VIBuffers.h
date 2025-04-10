#pragma once
#include "HAL/PlatformType.h"

#define _TCHAR_DEFINED
#include <d3d11.h>

class FVIBuffers
{
public:
    FVIBuffers()
        : VertexBuffer(nullptr), IndexBuffer(nullptr) {}
    
    void SetVertexBuffer(ID3D11Buffer* InVertexBuffer, const uint32 InStride, const uint32 InNumVertices, const D3D11_PRIMITIVE_TOPOLOGY InTopology) { VertexBuffer = InVertexBuffer; Stride = InStride; NumVertices = InNumVertices; Topology = InTopology; }
    void SetIndexBuffer(ID3D11Buffer* InIndexBuffer, const uint32 InNumIndices) { IndexBuffer = InIndexBuffer; numIndices = InNumIndices; }
    ID3D11Buffer* GetVertexBuffer() const { return VertexBuffer; }
    ID3D11Buffer* GetIndexBuffer() const { return IndexBuffer; }

    uint32 GetNumIndices() const { return numIndices; }
    uint32 GetStride() const { return Stride; }
    uint32 GetOffset() const { return Offset; }
    uint32 GetNumVertices() const { return NumVertices; }
    
    void Bind(ID3D11DeviceContext* context) const;
private:
    ID3D11Buffer* VertexBuffer = nullptr;
    uint32 Stride = 0;
    uint32 Offset = 0;
    uint32 NumVertices = 0;

    ID3D11Buffer* IndexBuffer = nullptr;
    uint32 numIndices = 0;

    D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

