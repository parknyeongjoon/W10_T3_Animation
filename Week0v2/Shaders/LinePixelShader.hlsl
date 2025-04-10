
struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

float4 mainPS(PS_INPUT input) : SV_Target
{
    return input.Color;
}
