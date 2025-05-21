#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <filesystem>

#include "UObject/NameTypes.h"

class FVBIBTopologyMapping
{
public:   
    void MappingVertexBuffer(const FName InVBName, const uint32 InStride, const uint32 InNumVertices, const D3D11_PRIMITIVE_TOPOLOGY InTopology) { VBName = InVBName; Stride = InStride; NumVertices = InNumVertices; Topology = InTopology; }
    void MappingIndexBuffer(const FName InIBName , const uint32 InNumIndices) { IBName = InIBName; numIndices = InNumIndices; }

    uint32 GetNumIndices() const { return numIndices; }
    uint32 GetStride() const { return Stride; }
    uint32 GetOffset() const { return Offset; }
    uint32 GetNumVertices() const { return NumVertices; }
    
    void Bind() const;
private:
    FName VBName = TEXT("");
    uint32 Stride = 0;
    uint32 Offset = 0;
    uint32 NumVertices = 0;
    std::filesystem::file_time_type VB_LastWriteTime;

    FName IBName= TEXT("");
    uint32 numIndices = 0;
    std::filesystem::file_time_type IB_LastWriteTime;

    D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

