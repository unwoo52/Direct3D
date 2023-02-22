struct PS_INPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

Texture2D tex : Texture : register(t0);
SamplerState samp : SMAPLER : register(s0);

float4 ps(PS_INPUT input) : SV_Target
{
    return tex.Sample(samp, input.uv);
}