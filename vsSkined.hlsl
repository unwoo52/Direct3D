#pragma pack_matrix(row_major)
#define MAX_BONE 256

cbuffer WVP : register(b0)
{
    matrix world;
    matrix viewProjection;
}

cbuffer BoneData : register(b1)
{
    matrix bones[MAX_BONE];
    int hasAnimation;
    int isChanging;
    float changingWeight;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float4 boneIDs : BONEIDS;
    float4 weights : WEIGHTS;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

matrix Skined(float4 boneIDs, float4 weights)
{
    matrix mat = 0;
    mat += mul(weights.x, bones[(uint) boneIDs.x]);
    mat += mul(weights.y, bones[(uint) boneIDs.y]);
    mat += mul(weights.z, bones[(uint) boneIDs.z]);
    mat += mul(weights.w, bones[(uint) boneIDs.w]);
    return mat;
}

PS_INPUT vs(VS_INPUT input)
{
    PS_INPUT output;
    matrix boneWorld = world;
    if(hasAnimation != 0)
    {
        boneWorld = mul(Skined(input.boneIDs, input.weights), world);
    }
    output.pos = mul(float4(input.pos, 1), mul(boneWorld, viewProjection));
    output.color = input.color;
    output.uv = input.uv;
    return output;
}