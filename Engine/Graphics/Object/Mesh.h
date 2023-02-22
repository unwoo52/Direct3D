#pragma once
#include "../Vertex.h"
#include "../Buffers.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>
#include "Texture.h"
#include "../../../ComException.h"

template<typename T>
class Mesh
{
	VertexBuffer<T> vb;
	IndexBuffer ib;
	ID3D11DeviceContext* dc;
	vector<Texture> textures;
	XMMATRIX local;
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* dc, vector<T>& vertices,
		vector<DWORD>& indices, vector<Texture>& textures, const XMMATRIX& mat)
	{
		this->dc = dc;
		this->textures = textures;
		local = mat;
		try
		{
			HRESULT hr = vb.Initialize(device, vertices.data(), vertices.size());
			COM_ERROR_IF_FAILED(hr, L"버텍스 버퍼 생성에 실패 하였습니다.");

			hr = ib.Initialize(device, indices.data(), indices.size());
			COM_ERROR_IF_FAILED(hr, L"인덱스 버퍼 생성에 실패 하였습니다.");
		}
		catch (ComException& ex)
		{
			Debug::Log(ex.what());
		}
	}
	Mesh(const Mesh& mesh)
	{
		dc = mesh.dc;
		vb = mesh.vb;
		ib = mesh.ib;
		textures = mesh.textures;
		local = mesh.local;
	}
	void Draw()
	{
		UINT offset = 0;

		for (Texture tex : textures)
		{
			if (tex.GetType() == aiTextureType::aiTextureType_DIFFUSE)
			{
				dc->PSSetShaderResources(0, 1, tex.GetTextureResourceViewAddress());
				break;
			}
		}

		dc->IASetVertexBuffers(0, 1, vb.GetAddressOf(), vb.StridePtr(), &offset);
		dc->IASetIndexBuffer(ib.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		dc->DrawIndexed(ib.Size(), 0, 0);
	}
	const XMMATRIX& GetLocal()
	{
		return local;
	}
};

