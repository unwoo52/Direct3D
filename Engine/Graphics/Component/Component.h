#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class GameObject;
class Transform;

class Component
{
protected:	
public:
	GameObject* gameObject = nullptr;
	virtual ~Component() {};
	virtual void Update() = 0;

	Transform* GetTransform();
};

