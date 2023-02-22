#include "GameObject.h"

GameObject::GameObject()
{
	shared_ptr<Transform> ptr = make_shared<Transform>();
	AddComponent(ptr);
}

Transform* GameObject::GetTransform()
{	
	return dynamic_cast<Transform*>(componentList.begin()->get());
}

void GameObject::AddComponent(shared_ptr<Component> com)
{	
	com->gameObject = this;
	componentList.push_back(com);
}

void GameObject::Update()
{
	for (shared_ptr<Component> comp : componentList)
	{
		comp->Update();
	}
}
