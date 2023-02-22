#pragma once
#include <memory>
#include <unordered_map>
#include "../Mesh.h"

struct BoneNode
{
	string name;
	int idx = 0;
	int parentIdx = 0;
	XMMATRIX offset = XMMatrixIdentity();
};

struct KeyScale
{
	float time;
	XMFLOAT3 scale;
};

struct KeyRotation
{
	float time;
	XMFLOAT4 rot;
};

struct KeyPosition
{
	float time;
	XMFLOAT3 pos;
};

struct AnimationNodeData
{
	string name;
	vector<KeyScale> scales;
	vector<KeyRotation> rotations;
	vector<KeyPosition> positions;
};

struct AnimationClipData
{
	string name;
	float duration;
	float tickPerSec;
	unordered_map<string, AnimationNodeData> keyFrames;
};

struct AnimationTransform
{
	XMFLOAT3 scale;
	XMFLOAT4 rot;
	XMFLOAT3 pos;
};

struct AnimationClip
{
	string name;
	float duration;
	float tickPerSec;
	unique_ptr<unique_ptr<AnimationTransform[]>[]> keyFrames; //[frame count][node count];
};

enum class ANIMSTATE
{
	LOOP, ONCE, STOP
};

struct AnimationState
{
	bool isPlaying = false;
	int idx = 0;
	int currentFrame = 1;
	int nextFrame = 2;
	float weight = 0.0f;
	ANIMSTATE state = ANIMSTATE::STOP;
};

#define MAX_BONE_LINK 4

struct SkinedVertexWeights
{
	UINT boneIDs[MAX_BONE_LINK] = { 0 };
	float weights[MAX_BONE_LINK] = { 0 };
	SkinedVertexWeights() {}
	SkinedVertexWeights(const SkinedVertexWeights& src)
	{
		memcpy(boneIDs, src.boneIDs, sizeof(boneIDs));
		memcpy(weights, src.weights, sizeof(weights));
	}
	void Add(const UINT& boneID, const float& weight)
	{
		for (UINT i = 0; i < MAX_BONE_LINK; ++i)
		{
			if (weights[i] == 0.0f)
			{
				boneIDs[i] = boneID;
				weights[i] = weight;
				break;
			}
		}
	}
	void Normalize()
	{
		float total = 0.0f;
		for (float weight : weights)
		{
			total += weight;
		}
		if (total == 0.0f) return;
		for (UINT i = 0; i < MAX_BONE_LINK; ++i)
		{
			if (weights[i] != 0.0f)
			{
				weights[i] /= total;
			}
		}
	}
};