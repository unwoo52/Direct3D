#pragma once
#include "Renderer.h"
#include "../../Interpolate.h"

class SkinedMeshRenderer : public Renderer
{
	//Animation
	unordered_map<string, UINT> boneMap; //»À´ë Á¤º¸
	vector<BoneNode> bones;
	vector<BoneNode> nodes;
	vector<AnimationClip> animClips;
	ConstantBuffer<CB_VS_BoneData> boneData;
	AnimationState animStateInfo;

	vector<Mesh<SkinedVertex>> meshes;

	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parent, UINT idx, UINT parentIdx);
	Mesh<SkinedVertex> ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& mat);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int idx, aiTextureType type);
	vector<Texture> LoadMaterialTextures(aiMaterial* pMat, aiTextureType type, const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);

	void ProcessBone(aiMesh* mesh, vector<SkinedVertexWeights>& weights);
	void LoadAnimation(const aiScene* pScene, vector<AnimationClipData>& animDatas);
	void MakeAnimationClip(vector<AnimationClipData>& animDatas);
	void ProcessKeyFrameAnimation();

	bool LoadFile(const string& filePath) override;
public:
	void Draw(const XMMATRIX vpMat) override;
	void Update() override;
};

