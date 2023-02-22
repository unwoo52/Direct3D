#pragma once
#include <DirectXMath.h>
#include "Component.h"
using namespace DirectX;

enum class CameraProjectionType
{
	Perspective, Ortho
};

class Camera : public Component
{	
	XMMATRIX view, proj;	
	void UpdateViewMatrix();
public:
	CameraProjectionType type;
	void SetPerspectiveProjection(float fov, float aspect, float near, float far);
	void SetOrthoProjection(float w, float h, float near, float far);
	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;
	const XMMATRIX GetViewProjMatrix() const;	
	virtual void Update() override;
};

