#pragma once
#include "../DebugLog.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
using namespace Microsoft::WRL;

class VertexShader
{
	ComPtr<ID3D11VertexShader> shader;
	ComPtr<ID3D10Blob> buffer;
	ComPtr<ID3D11InputLayout> inputLayout;
public:
	bool Initialize(ID3D11Device* device, wstring path, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();
};

class PixelShader
{
	ComPtr<ID3D11PixelShader> shader;
	ComPtr<ID3D10Blob> buffer;
public:
	bool Initialize(ID3D11Device* device, wstring path);
	ID3D11PixelShader* GetShader();
	ID3D10Blob* GetBuffer();
};

