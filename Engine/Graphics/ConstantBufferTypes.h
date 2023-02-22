#pragma once
#include <DirectXMath.h>
#include "Object/Animation/BoneAnimation.h"
using namespace DirectX;

struct CB_VS_Offset
{
	XMFLOAT2 offset;
};

struct CB_VS_Transform
{
	XMMATRIX world;
	XMMATRIX vp;
};

struct CB_VS_Transform2D
{
	XMMATRIX matWVP;
};

struct CB_PS_Alpha
{
	float alpha;
};

struct CB_PS_Light
{		
	XMFLOAT3 ambientColor;
	float ambientStrength;
	XMFLOAT3 lightColor;
	float lightStrength;
	XMFLOAT3 lightDir;
};

struct CB_VS_BoneData
{
	XMMATRIX bones[256];
	int hasAnimation;
	int isChanging;
	float changingWeight;
};
