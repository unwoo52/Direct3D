#pragma once
#include "../Vertex.h"
#include "../Buffers.h"
#include "../ConstantBufferTypes.h"
#include "Component.h"

class Renderer : public Component
{
protected:
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* dc = nullptr;
	ConstantBuffer<CB_VS_Transform>* cb_transform = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;	
	//for Assimp
	string directory = "";	
	virtual bool LoadFile(const string& filePath) = 0;	
public:
	virtual void Update() override;	
	bool Initialize(const string& filePath, ID3D11Device* device, ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, ConstantBuffer<CB_VS_Transform>& cb_vs_transform);
	void SetTexture(ID3D11ShaderResourceView* tex);
	virtual void Draw(const XMMATRIX vpMat) = 0;	
};

