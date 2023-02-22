#pragma once
#include "Graphics/Object/Animation/BoneAnimation.h"

typedef unordered_map<string, AnimationNodeData>::iterator KeyFramesIter;

static float Clamp(float v, float min, float max)
{
	return v > max ? max : v < min ? min : v;
}

static int FindScale(KeyFramesIter iter, float time)
{
	auto& list = iter->second.scales;
	if (list.empty()) return -1;
	for (UINT i = 0; i < list.size(); ++i)
	{
		if (time < list[i + 1].time) return i;
	}
	return -1;
}

static int FindPos(KeyFramesIter iter, float time)
{
	auto& list = iter->second.positions;
	if (list.empty()) return -1;
	for (UINT i = 0; i < list.size(); ++i)
	{
		if (time < list[i + 1].time) return i;
	}
	return -1;
}

static int FindRot(KeyFramesIter iter, float time)
{
	auto& list = iter->second.rotations;
	if (list.empty()) return -1;
	for (UINT i = 0; i < list.size(); ++i)
	{
		if (time < list[i + 1].time) return i;
	}
	return -1;
}

static XMFLOAT3* Vec3Linear(XMFLOAT3* pOut, const XMFLOAT3* src, const XMFLOAT3* des, float t)
{
	if (pOut == nullptr || src == nullptr || des == nullptr) return nullptr;
	t = Clamp(t, 0.0f, 1.0f);

	XMVECTOR vOut, vSrc, vDes;
	vOut = XMLoadFloat3(pOut);
	vSrc = XMLoadFloat3(src);
	vDes = XMLoadFloat3(des);
	vOut = vSrc + (vDes - vSrc) * t;
	XMStoreFloat3(pOut, vOut);
	return pOut;
}

static XMFLOAT3 CalcInterpolatedPosition(KeyFramesIter iter, float t, int dur)
{
	auto frame = iter->second.positions;
	if (frame.empty()) return { 0, 0, 0 };
	if (frame.size() == 1) return frame.front().pos;
	if (t == dur - 1) return frame.back().pos;
	int Idx = FindPos(iter, t);
	if (Idx == -1) return frame.back().pos;

	UINT nextIdx = Idx + 1;
	assert(nextIdx < frame.size());

	float deltaTime = frame[nextIdx].time - frame[Idx].time;
	float factor = (t - frame[Idx].time) / deltaTime;

	if (factor < 0.0f) return frame.front().pos;

	auto start = frame[Idx].pos;
	auto end = frame[nextIdx].pos;

	Vec3Linear(&start, &start, &end, factor);
	return start;
}

static XMFLOAT4 CalcInterpolatedRotation(KeyFramesIter iter, float t, int dur)
{
	auto frame = iter->second.rotations;
	if (frame.empty()) return { 0, 0, 0, 1 };
	if (frame.size() == 1) return frame.front().rot;
	if (t == dur - 1) return frame.back().rot;
	int Idx = FindRot(iter, t);
	if (Idx == -1) return frame.back().rot;

	UINT nextIdx = Idx + 1;
	assert(nextIdx < frame.size());

	float deltaTime = frame[nextIdx].time - frame[Idx].time;
	float factor = (t - frame[Idx].time) / deltaTime;

	if (factor < 0.0f) return frame.front().rot;

	auto start = frame[Idx].rot;
	auto end = frame[nextIdx].rot;

	XMVECTOR vStart, vEnd;
	vStart = XMLoadFloat4(&start);
	vEnd = XMLoadFloat4(&end);
	vStart = XMQuaternionSlerp(vStart, vEnd, factor);
	XMStoreFloat4(&start, vStart);
	return start;
}

static XMFLOAT3 CalcInterpolatedScale(KeyFramesIter iter, float t, int dur)
{
	auto frame = iter->second.scales;
	if (frame.empty()) return { 0, 0, 0 };
	if (frame.size() == 1) return frame.front().scale;
	if (t == dur - 1) return frame.back().scale;
	int Idx = FindPos(iter, t);
	if (Idx == -1) return frame.back().scale;

	UINT nextIdx = Idx + 1;
	assert(nextIdx < frame.size());

	float deltaTime = frame[nextIdx].time - frame[Idx].time;
	float factor = (t - frame[Idx].time) / deltaTime;

	if (factor < 0.0f) return frame.front().scale;

	auto start = frame[Idx].scale;
	auto end = frame[nextIdx].scale;

	Vec3Linear(&start, &start, &end, factor);
	return start;
}