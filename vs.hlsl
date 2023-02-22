#pragma pack_matrix( row_major )

cbuffer transform : register(b0)
{
    float4x4 world;
    float4x4 vp;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

PS_INPUT vs(VS_INPUT inPut)
{
    PS_INPUT outPut;
        
    outPut.pos = mul(float4(inPut.pos, 1.0f), mul(world, vp));
    outPut.color = inPut.color;
    outPut.uv = inPut.uv;
    outPut.normal = normalize(mul(float4(inPut.normal, 0.0f), world));
	return outPut;
}