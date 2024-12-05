#pragma once
#include <memory>

#include "AnimatedSprite.h"
#include "BubbleObject.h"
#include "NailObject.h"
#include "Settings.h"

class BubbleWrapper
{
public:

	void Update()
	{


		for(size_t i = 0; i < m_GameObjects.size(); i++)
		{
			sf::Vector2f position = m_GameObjects[i]->GetPosition();
			position.x *= Settings::get().GetPixelToMeter();
			position.y *= Settings::get().GetPixelToMeter();
			position.y *= -1;
			m_Rendered[i]->SetPosition(position);
			m_Rendered[i]->SetRotation(m_GameObjects[i]->GetRotation() * 57.2957795f);
		}
	}

	size_t GetNumOfObjects() const { return m_GameObjects.size(); }

	AnimatedSprite* GetShapeByIndex(size_t a_Index) const { return m_Rendered[a_Index].get(); }
	GameObject* GetObjectByIndex(size_t a_Index) const { return m_GameObjects[a_Index].get(); }

	void AddObject(std::unique_ptr<GameObject> a_GameObject, std::unique_ptr<AnimatedSprite> a_Rendered)
	{
		m_GameObjects.push_back(std::move(a_GameObject));
		m_Rendered.push_back(std::move(a_Rendered));
	}
	void RemoveObjectByIndex(size_t a_Index)
	{
		m_GameObjects.erase(m_GameObjects.begin() + a_Index);
		m_Rendered.erase(m_Rendered.begin() + a_Index);
	}
	void RemoveObjectByPointer(const BubbleObject* a_Bubble)
	{
		for(size_t i = 0; i < m_GameObjects.size(); i++)
		{
			if(m_GameObjects[i].get() == a_Bubble)
			{
				m_GameObjects.erase(m_GameObjects.begin() + i);
				m_Rendered.erase(m_Rendered.begin() + i);
				break;
			}
		}
	}

	void Clear()
	{
		m_GameObjects.clear();
		m_Rendered.clear();
	}

	std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; }
	std::vector<std::unique_ptr<AnimatedSprite>>& GetRendered() { return m_Rendered; }

private:
	std::vector<std::unique_ptr<AnimatedSprite>> m_Rendered;
	std::vector<std::unique_ptr<GameObject>> m_GameObjects;

};
