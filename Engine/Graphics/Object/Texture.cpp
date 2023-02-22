#include "Texture.h"
#include "../../../ComException.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

string GetFileExtension(const string& filename)
{
    size_t off = filename.find_last_of('.');
    if (off == string::npos)
    {
        return {};
    }
    return string(filename.substr(off + 1));
}

wstring StringToWide(const string& str)
{
    return wstring(str.begin(), str.end());
}

void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const ColorClass& color, aiTextureType type)
{
    InitializeColorTexture(device, &color, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device* device, const ColorClass* color, UINT w, UINT h, aiTextureType type)
{
    this->type = type;
    CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, w, h);
    ID3D11Texture2D* p2DTexture = nullptr;
    D3D11_SUBRESOURCE_DATA initialData = { 0 };
    initialData.pSysMem = color;
    initialData.SysMemPitch = w * sizeof(ColorClass);
    try
    {
        HRESULT hr = device->CreateTexture2D(&desc, &initialData, &p2DTexture);
        COM_ERROR_IF_FAILED(hr, L"컬러 텍스쳐 생성에 실패 하였습니다.");
        texture = static_cast<ID3D11Texture2D*>(p2DTexture);
        CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format);
        hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"Shader Resource View 생성에 실패 하였습니다.");
    }
    catch(ComException& ex)
    {
        Debug::Log(ex.what());
    }
}

Texture::Texture(ID3D11Device* device, const ColorClass& color, aiTextureType type)
{
    Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, const ColorClass* color, UINT w, UINT h, aiTextureType type)
{
    InitializeColorTexture(device, color, w, h, type);
}

Texture::Texture(ID3D11Device* device, const string& filePath, aiTextureType type)
{
    try
    {
        this->type = type;
        HRESULT hr = { 0 };
        if (GetFileExtension(filePath) == "dds")
        {
            hr = DirectX::CreateDDSTextureFromFile(device, StringToWide(filePath).c_str(), texture.GetAddressOf(), textureView.GetAddressOf());
            COM_ERROR_IF_FAILED(hr, L"dds 파일을 읽어 오지 못했습니다.");
        }
        else
        {
            hr = DirectX::CreateWICTextureFromFile(device, StringToWide(filePath).c_str(), texture.GetAddressOf(), textureView.GetAddressOf());
            COM_ERROR_IF_FAILED(hr, L"텍스쳐 파일을 읽어 오지 못했습니다.");
        }
    }
    catch (ComException& ex)
    {
        Debug::Log(ex.what());
        Initialize1x1ColorTexture(device, COLORS::UnloadedTextureColor, type);        
    }
}

Texture::Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type)
{
    this->type = type;
    try
    {
        HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, texture.GetAddressOf(), textureView.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"텍스쳐 생성에 실패 하였습니다.");
    }
    catch (ComException& ex)
    {
        Debug::Log(ex.what());
    }
}

aiTextureType Texture::GetType()
{
    return type;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
    return textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
    return textureView.GetAddressOf();
}
