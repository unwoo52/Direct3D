#pragma once
#include "../Component/ComponentTypes.h"
#include <list>
#include <vector>
#include <memory>
#include <unordered_map>
using namespace std;

class GameObject
{
	list<shared_ptr<Component>> componentList;
	unordered_map<string, Component*> components;
public:
	GameObject();
	Transform* GetTransform();
	void AddComponent(shared_ptr<Component> com);
	template<class T>
	T* GetComponent()
	{
		if (components.find(typeid(T).name()) == components.end())
		{
			T* ptr = nullptr;
			for (shared_ptr<Component> comp : componentList)
			{
				ptr = dynamic_cast<T*>(comp.get());
				if (ptr != nullptr)
				{
					components[typeid(T).name()] = comp.get();
					break;
				}
			}
			if (ptr == nullptr)
			{
				return nullptr;
			}			
		}
		return dynamic_cast<T*>(components[typeid(T).name()]);
	}
	void Update();
	void Draw(XMMATRIX vp)
	{
		GetComponent<Renderer>()->Draw(vp);
	}
};

