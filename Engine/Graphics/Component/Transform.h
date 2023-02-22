#pragma once
#include "Component.h"

class Transform : public Component
{
	XMMATRIX world = XMMatrixIdentity();
	XMVECTOR posVector = { 0 }, rotVector = { 0 };
	XMFLOAT3 pos = { 0, 0, 0 }, rot = { 0, 0, 0 }, scale = { 1, 1, 1 };
	XMVECTOR forward = DEFAULT_FORWARD_VECTOR, 
		backward = DEFAULT_BACKWARD_VECTOR,
		right = DEFAULT_RIGHT_VECTOR, 
		left = DEFAULT_LEFT_VECTOR, 
		up = DEFAULT_UP_VECTOR;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	void UdateWorldMatrix();
public:	
	Transform();	
	void Update() override;
	XMMATRIX GetWorld();

	const XMVECTOR& GetPosVector() const;
	const XMFLOAT3& GetPos() const;
	const XMVECTOR& GetRotVector() const;
	const XMFLOAT3& GetRot() const;
	const XMFLOAT3& GetScale() const;

	const XMVECTOR& GetForward();
	const XMVECTOR& GetBackward();
	const XMVECTOR& GetRight();
	const XMVECTOR& GetLeft();
	const XMVECTOR& GetUp();
	const XMVECTOR& GetDown();

	void SetPos(const XMVECTOR& posVector);
	void SetPos(float x, float y, float z);
	void SetPos(XMFLOAT3& pos);

	void Translate(const XMVECTOR& posVector);
	void Translate(float x, float y, float z);
	void Translate(XMFLOAT3& pos);

	void SetRot(const XMVECTOR& posVector);
	void SetRot(float x, float y, float z);
	void SetRot(XMFLOAT3& pos);

	void Rotate(const XMVECTOR& posVector);
	void Rotate(float x, float y, float z);
	void Rotate(XMFLOAT3& pos);

	void SetScale(float x, float y, float z);
	void SetScale(XMFLOAT3& scale);

	void LookAtPos(XMFLOAT3 pos);
	void LookAtPos(float x, float y, float z);
};

