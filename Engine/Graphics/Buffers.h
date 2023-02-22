#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "../DebugLog.h"
using namespace Microsoft::WRL;

class Buffer
{
protected:
	ComPtr<ID3D11Buffer> buffer;
	UINT size = 0;
public:
	ID3D11Buffer* Get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}

	UINT Size() const
	{
		return size;
	}
};

template<class T>
class VertexBuffer : public Buffer
{	
	UINT stride = 0;	
public:
	VertexBuffer() {};	

	VertexBuffer<T>& operator=(const VertexBuffer<T>& right)
	{
		buffer = right.buffer;
		size = right.size;
		stride = right.stride;
		return *this;
	}

	const UINT Stride() const
	{
		return stride;
	}
	
	const UINT* StridePtr() const
	{
		return &stride;
	}

	HRESULT Initialize(ID3D11Device* device, T* data, UINT numVertices)
	{
		if (buffer.Get() != nullptr) buffer.Reset();

		size = numVertices;
		stride = sizeof(T);

		D3D11_BUFFER_DESC vbDesc = { 0 };
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.ByteWidth = sizeof(T) * numVertices;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = 0;
		vbDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vbData = { 0 };
		vbData.pSysMem = data;

		return device->CreateBuffer(&vbDesc, &vbData, buffer.GetAddressOf());		
	}
};

class IndexBuffer : public Buffer
{	
public:
	IndexBuffer() {};	

	HRESULT Initialize(ID3D11Device* device, DWORD* data, UINT numIndices)
	{
		if (buffer.Get() != nullptr) buffer.Reset();

		size = numIndices;

		D3D11_BUFFER_DESC ibDesc = { 0 };
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = sizeof(DWORD) * numIndices;
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA ibData = { 0 };
		ibData.pSysMem = data;

		return device->CreateBuffer(&ibDesc, &ibData, buffer.GetAddressOf());		
	}
};

template<class T>
class ConstantBuffer : public Buffer
{
	ID3D11DeviceContext* dc = nullptr;
public:
	T data;
	ConstantBuffer() {}

	HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* dc)
	{
		if (buffer.Get() != nullptr) buffer.Reset();

		this->dc = dc;

		//Create Constant Buffer
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.ByteWidth = sizeof(T) + (16 - sizeof(T) % 16);
		cbDesc.StructureByteStride = 0;

		return device->CreateBuffer(&cbDesc, 0, buffer.GetAddressOf());
	}

	bool Update()
	{
		D3D11_MAPPED_SUBRESOURCE map;
		HRESULT hr = dc->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		if (FAILED(hr))
		{
			Debug::Log(hr, L"콘스탄트 버퍼를 사용할 수 없습니다.");
			return false;
		}
		CopyMemory(map.pData, &data, sizeof(T));
		dc->Unmap(buffer.Get(), 0);
		return true;
	}
};