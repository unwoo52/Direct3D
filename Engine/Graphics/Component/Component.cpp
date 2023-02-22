#include "Component.h"
#include "../Object/GameObject.h"

Transform* Component::GetTransform()
{
    return gameObject->GetTransform();
}
