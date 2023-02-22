#include "SpriteRenderer.h"
#include "../../../ComException.h"
#include "../Object/GameObject.h"

bool SpriteRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, float w, float h, string path, ConstantBuffer<CB_VS_Transform2D>& cbTransform)
{
    this->w = w;
    this->h = h;
    this->dc = dc;
    if (dc == nullptr) return false;
    this->cbTransform = &cbTransform;
    tex = make_unique<Texture>(device, path, aiTextureType::aiTextureType_DIFFUSE);

    Vertex2D vertices[] =
    {
        Vertex2D(-0.5f, -0.5f, 0.0f, 0, 0),
        Vertex2D(0.5f, -0.5f, 0.0f, 1, 0),
        Vertex2D(-0.5f, 0.5f, 0.0f, 0, 1),
        Vertex2D(0.5f, 0.5f, 0.0f, 1, 1)
    };

    DWORD indices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    try
    {
        HRESULT hr = vb.Initialize(device, vertices, ARRAYSIZE(vertices));
        COM_ERROR_IF_FAILED(hr, L"버텍스 버퍼 생성에 실패 하였습니다.");

        hr = ib.Initialize(device, indices, ARRAYSIZE(indices));
        COM_ERROR_IF_FAILED(hr, L"인덱스 버퍼 생성에 실패 하였습니다.");
    }
    catch (ComException& ex)
    {
        Debug::Log(ex.what());
        return false;
    }   

    return true;
}

void SpriteRenderer::Draw(XMMATRIX ortho)
{
    cbTransform->data.matWVP = GetTransform()->GetWorld() * ortho;
    cbTransform->Update();
    dc->VSSetConstantBuffers(0, 1, cbTransform->GetAddressOf());
    dc->PSSetShaderResources(0, 1, tex->GetTextureResourceViewAddress());
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, vb.GetAddressOf(), vb.StridePtr(), &offset);
    dc->IASetIndexBuffer(ib.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    dc->DrawIndexed(ib.Size(), 0, 0);
}

float SpriteRenderer::GetWidth()
{
    return w;
}

float SpriteRenderer::GetHeight()
{
    return h;
}

void SpriteRenderer::Update()
{    
}

void SpriteRenderer::SetTransform()
{
    GetTransform()->SetPos(0, 0, 0);
    GetTransform()->SetRot(0, 0, 0);
    GetTransform()->SetScale(w, h, 1);
}
