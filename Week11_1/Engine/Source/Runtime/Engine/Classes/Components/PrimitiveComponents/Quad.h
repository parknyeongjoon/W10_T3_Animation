#pragma once
#include "Define.h"
FVertexSimple quadVertices[] =
{
    {-1.0f,1.0f,0.0f, 1.0f, 0.0f,1.0f,0.0f,1.0f},
    { 1.0f,1.0f,0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f},
    {-1.0f,-1.0f,0.0f, 1.0f, 1.0f,1.0f,0.0f,1.0f},
    { 1.0f,-1.0f,0.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f}
};

uint32 quadInices[] =
{
    0,1,2,
    1,3,2
};

