#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 color;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	Vertex() : pos(0,0,0),color(1,1,1),uv(0,0),normal(0,0,0) {}
	Vertex(float x, float y, float z, XMFLOAT2 uv) : pos(x, y, z), color(1, 1, 1), uv(uv), normal(0, 0, 0) {}
	Vertex(float x, float y, float z, float u, float v) : pos(x,y,z), color(1,1,1), uv(u,v), normal(0, 0, 0) {}
	Vertex(float x, float y, float z, XMFLOAT3 color, XMFLOAT2 uv) : pos(x,y,z),color(color), uv(uv), normal(0, 0, 0) {}
};

struct Vertex2D
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	Vertex2D() : pos(0,0,0), uv(0,0) {}
	Vertex2D(float x, float y, float z, float u, float v) : pos(x,y,z), uv(u,v) {}
};

struct SkinedVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 color;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT4 boneIDs;
	XMFLOAT4 weights;
	SkinedVertex() : pos(0, 0, 0), color(1, 1, 1), uv(0, 0), normal(0, 0, 0), boneIDs(0, 0, 0, 0), weights(0, 0, 0, 0) {}
	SkinedVertex(XMFLOAT3 pos, XMFLOAT2 uv) : pos(pos), color(1,1,1), uv(uv), normal(0, 0, 0), boneIDs(0, 0, 0, 0), weights(0, 0, 0, 0) {}
};
