#include "Renderer.h"
#include "../../../ComException.h"
#include "../Object/GameObject.h"

void Renderer::Update()
{    
}

bool Renderer::Initialize(const string& filePath, ID3D11Device* device, ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, ConstantBuffer<CB_VS_Transform>& cb_vs_transform)
{
    this->device = device;
    this->dc = dc;
    this->texture = tex;
    this->cb_transform = &cb_vs_transform;

    if (!LoadFile(filePath))
    {
        Debug::Log(L"잘못된 파일 입니다.");
        return false;
    }
    return true;
}

void Renderer::SetTexture(ID3D11ShaderResourceView* tex)
{
    texture = tex;
}