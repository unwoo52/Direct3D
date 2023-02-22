#pragma once
#include "Component.h"
#include "../Object/Texture.h"
#include "../ConstantBufferTypes.h"
#include "../Buffers.h"
#include "../Vertex.h"
#include <memory>
using namespace std;

class SpriteRenderer : public Component
{
	unique_ptr<Texture> tex;
	ID3D11DeviceContext* dc = nullptr;
	ConstantBuffer<CB_VS_Transform2D>* cbTransform;
	IndexBuffer ib;
	VertexBuffer<Vertex2D> vb;
	float w, h;
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, 
		float w, float h, string path, ConstantBuffer<CB_VS_Transform2D>& cbTransform);
	void Draw(XMMATRIX ortho);
	float GetWidth();
	float GetHeight();
	virtual void Update() override;
	void SetTransform();
};

