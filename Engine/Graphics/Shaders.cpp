#include "Shaders.h"

bool VertexShader::Initialize(ID3D11Device* device, wstring path, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(path.c_str(), buffer.GetAddressOf());
	if (FAILED(hr))
	{
		wstring msg = L"Shader�� �о� ���µ� ���� �Ͽ����ϴ�.";
		msg += L"\n path : ";
		msg += path;
		Debug::Log(hr, msg);
		return false;
	}

	hr = device->CreateVertexShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		wstring msg = L"Shader�� ���� �ϴµ� ���� �Ͽ����ϴ�.";
		msg += L"\n path : ";
		msg += path;
		Debug::Log(hr, msg);
		return false;
	}

	hr = device->CreateInputLayout(layout, numElements, buffer->GetBufferPointer(), buffer->GetBufferSize(), inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		Debug::Log(hr, L"Input Layout ������ ���� �Ͽ����ϴ�.");
		return false;
	}

	return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer()
{
	return buffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return inputLayout.Get();
}

bool PixelShader::Initialize(ID3D11Device* device, wstring path)
{
	HRESULT hr = D3DReadFileToBlob(path.c_str(), buffer.GetAddressOf());
	if (FAILED(hr))
	{
		wstring msg = L"Shader�� �о� ���µ� ���� �Ͽ����ϴ�.";
		msg += L"\n path : ";
		msg += path;
		Debug::Log(hr, msg);
		return false;
	}

	hr = device->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		wstring msg = L"Shader�� ���� �ϴµ� ���� �Ͽ����ϴ�.";
		msg += L"\n path : ";
		msg += path;
		Debug::Log(hr, msg);
		return false;
	}

	return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer()
{
	return buffer.Get();
}
