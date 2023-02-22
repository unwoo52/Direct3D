#include "Camera.h"
#include "../Object/GameObject.h"
void Camera::UpdateViewMatrix()
{
    XMVECTOR eyeFocus = GetTransform()->GetPosVector() + GetTransform()->GetForward();    
    view = XMMatrixLookAtLH(GetTransform()->GetPosVector(), eyeFocus, GetTransform()->GetUp());
}

void Camera::SetPerspectiveProjection(float fov, float aspect, float nearZ, float farZ)
{
    type = CameraProjectionType::Perspective;
    float rad = (fov / 360.0f) * XM_2PI;
    proj = XMMatrixPerspectiveFovLH(rad, aspect, nearZ, farZ);
}

void Camera::SetOrthoProjection(float w, float h, float nearZ, float farZ)
{
    type = CameraProjectionType::Ortho;
    proj = XMMatrixOrthographicOffCenterLH(0.0f, w, h, 0.0f, nearZ, farZ);
}

const XMMATRIX& Camera::GetViewMatrix() const
{
    return view;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
    return proj;
}

const XMMATRIX Camera::GetViewProjMatrix() const
{
    return view * proj;    
}

void Camera::Update()
{
    UpdateViewMatrix();
}
