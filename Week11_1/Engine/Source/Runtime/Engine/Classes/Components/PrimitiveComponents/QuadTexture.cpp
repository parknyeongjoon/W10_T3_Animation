#include "QuadTexture.h"

uint32 quadTextureInices[] =
{
    0,1,2,
    1,3,2
};

//Direct3d11 기준 쿼드 작성 (uv에서 v반전 필요없음)
FVertexTexture quadTextureVertices[] =
{
    {   -1.0f,  1.0f,   0.0f,   0.0f,0.0f},
    {    1.0f,  1.0f,   0.0f,   1.0f,1.0f},
    {   -1.0f,  -1.0f,  0.0f,   0.0f,1.0f},
    {    1.0f,  -1.0f,  0.0f,   1.0f,0.0f}
};