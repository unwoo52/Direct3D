#pragma pack_matrix( row_major )

cbuffer Transform : register(b0)
{
    float4x4 matWVP;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

PS_INPUT vs(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(float4(input.pos, 1.0f), matWVP);
    output.uv = input.uv;
    return output;
}