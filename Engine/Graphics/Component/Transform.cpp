#include "Transform.h"
#include "../Object/GameObject.h"
void Transform::UdateWorldMatrix()
{
    XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
    world = XMMatrixScaling(scale.x, scale.y, scale.z) * rotMat * XMMatrixTranslation(pos.x, pos.y, pos.z);
    forward = XMVector3TransformNormal(DEFAULT_FORWARD_VECTOR, rotMat);
    backward = XMVector3TransformNormal(DEFAULT_BACKWARD_VECTOR, rotMat);
    right = XMVector3TransformNormal(DEFAULT_RIGHT_VECTOR, rotMat);
    left = XMVector3TransformNormal(DEFAULT_LEFT_VECTOR, rotMat);
    up = XMVector3TransformNormal(DEFAULT_UP_VECTOR, rotMat); 
    gameObject->Update();
}

Transform::Transform()
{
}

void Transform::Update()
{    
}

XMMATRIX Transform::GetWorld()
{
    return world;
}

const XMVECTOR& Transform::GetPosVector() const
{
    return posVector;
}

const XMFLOAT3& Transform::GetPos() const
{
    return pos;
}

const XMVECTOR& Transform::GetRotVector() const
{
    return rotVector;
}

const XMFLOAT3& Transform::GetRot() const
{
    return rot;
}

const XMFLOAT3& Transform::GetScale() const
{
    return scale;
}

const XMVECTOR& Transform::GetForward()
{
    return forward;
}

const XMVECTOR& Transform::GetBackward()
{
    return backward;
}

const XMVECTOR& Transform::GetRight()
{
    return right;
}

const XMVECTOR& Transform::GetLeft()
{
    return left;
}

const XMVECTOR& Transform::GetUp()
{
    return up;
}

const XMVECTOR& Transform::GetDown()
{
    return -up;
}

void Transform::SetPos(const XMVECTOR& posVector)
{
    this->posVector = posVector;
    XMStoreFloat3(&pos, posVector);
    UdateWorldMatrix();
}

void Transform::SetPos(float x, float y, float z)
{
    pos = { x, y, z };
    posVector = XMLoadFloat3(&pos);
    UdateWorldMatrix();
}

void Transform::SetPos(XMFLOAT3& pos)
{
    SetPos(pos.x, pos.y, pos.z);
}

void Transform::Translate(const XMVECTOR& posVector)
{
    this->posVector += posVector;
    XMStoreFloat3(&pos, this->posVector);
    UdateWorldMatrix();
}

void Transform::Translate(float x, float y, float z)
{
    pos.x += x;
    pos.y += y;
    pos.z += z;
    posVector = XMLoadFloat3(&pos);
    UdateWorldMatrix();
}

void Transform::Translate(XMFLOAT3& pos)
{
    Translate(pos.x, pos.y, pos.z);
}

void Transform::SetRot(const XMVECTOR& posVector)
{
    this->rotVector = rotVector;
    XMStoreFloat3(&rot, rotVector);
    UdateWorldMatrix();
}

void Transform::SetRot(float x, float y, float z)
{
    rot = { x, y, z };
    rotVector = XMLoadFloat3(&rot);
    UdateWorldMatrix();
}

void Transform::SetRot(XMFLOAT3& pos)
{
    SetRot(rot.x, rot.y, rot.z);
}

void Transform::Rotate(const XMVECTOR& posVector)
{
    this->rotVector += rotVector;
    XMStoreFloat3(&rot, this->rotVector);
    UdateWorldMatrix();
}

void Transform::Rotate(float x, float y, float z)
{
    rot.x += x;
    rot.y += y;
    rot.z += z;
    rotVector = XMLoadFloat3(&rot);
    UdateWorldMatrix();
}

void Transform::Rotate(XMFLOAT3& pos)
{
    Rotate(rot.x, rot.y, rot.z);
}

void Transform::SetScale(float x, float y, float z)
{
    scale = { x,y,z };
    UdateWorldMatrix();
}

void Transform::SetScale(XMFLOAT3& scale)
{
    SetScale(scale.x, scale.y, scale.z);
}

void Transform::LookAtPos(XMFLOAT3 pos)
{
    LookAtPos(pos.x, pos.y, pos.z);
}

void Transform::LookAtPos(float x, float y, float z)
{
    double dx = (double)pos.x - x;
    double dy = (double)pos.y - y;
    double dz = (double)pos.z - z;

    float yaw = 0.0f;
    if (dz > DBL_EPSILON || dz < -DBL_EPSILON)
    {
        yaw = atan(dx / dz);
        if (dz > 0.0f)
        {
            yaw += XM_PI;
        }
    }

    float pitch = 0.0f;
    float dist = sqrt(dx * dx + dz * dz);
    if (dist > DBL_EPSILON || dist < -DBL_EPSILON)
    {
        pitch = atan(dy / dist);
    }
    SetRot(pitch, yaw, 0.0f);
}
