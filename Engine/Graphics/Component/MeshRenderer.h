#pragma once
#include "Renderer.h"
class MeshRenderer : public Renderer
{	
	VertexBuffer<Vertex> vb;
	IndexBuffer ib;
	//for Assimp	
	vector<Mesh<Vertex>> meshes;
	bool LoadFile(const string& filePath) override;
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parent);
	Mesh<Vertex> ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& mat);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int idx, aiTextureType type);
	vector<Texture> LoadMaterialTextures(aiMaterial* pMat, aiTextureType type, const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);
public:
	void Update() override;
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, ConstantBuffer<CB_VS_Transform>& cb_vs_transform);	
	void Draw(const XMMATRIX vpMat) override;
};

