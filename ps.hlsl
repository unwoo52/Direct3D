cbuffer Alpha : register(b0)
{
    float alpha;
}

cbuffer LightBuffer : register(b1)
{      
    float3 ambientColor;
    float ambientStrength;
    float3 lightColor;
    float lightStrength;
    float3 lightDir;
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

Texture2D tex : TEXTURE : register(t0);
SamplerState texSample : SAMPLER : register(s0);

float4 ps(PS_INPUT inPut) : SV_TARGET
{
    float3 texColor = tex.Sample(texSample, inPut.uv);
    float3 color = inPut.color;
    color = ambientColor * ambientStrength;
    //����� ���� �� ������ ����
    float3 dir = normalize(-lightDir);
    //���������� ���� ���� �󸶳� �޴��� ��� ( ���ݻ� )
    float lightIntensity = dot(inPut.normal, dir);    
    if(lightIntensity > 0)
    {
        color += lightColor * lightIntensity * lightStrength;        
    }       
    color = saturate(color * texColor);
    return float4(color, alpha);
}