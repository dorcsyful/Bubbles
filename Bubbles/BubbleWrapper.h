#pragma once
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>

#include "AnimatedSprite.h"
#include "BubbleObject.h"

class BubbleWrapper
{
public:

	void Update()
	{
		if(m_GameBubble.size() != m_Rendered.size())
		{
			throw std::runtime_error("BubbleWrapper::Update() - m_GameBubble and m_Rendered size mismatch");
		}

		for(size_t i = 0; i < m_GameBubble.size(); i++)
		{
			sf::Vector2f position = m_GameBubble[i]->GetPosition();
			position.x *= PIXEL_TO_METER;
			position.y *= PIXEL_TO_METER;
			position.y *= -1;
			m_Rendered[i]->SetPosition(position);
			m_Rendered[i]->SetRotation(m_GameBubble[i]->GetRotation() * 57.2957795f);
		}
	}

	size_t GetNumOfBubbles() const { return m_GameBubble.size(); }

	AnimatedSprite* GetShapeByIndex(size_t a_Index) const { return m_Rendered[a_Index].get(); }
	BubbleObject* GetBubbleByIndex(size_t a_Index) const { return m_GameBubble[a_Index].get(); }

	void AddBubble(std::unique_ptr<BubbleObject> a_Bubble, std::unique_ptr<AnimatedSprite> a_Rendered)
	{
		m_GameBubble.push_back(std::move(a_Bubble));
		m_Rendered.push_back(std::move(a_Rendered));
	}
	void RemoveBubbleByIndex(size_t a_Index)
	{
		m_GameBubble.erase(m_GameBubble.begin() + a_Index);
		m_Rendered.erase(m_Rendered.begin() + a_Index);
	}
	void RemoveBubbleByPointer(const BubbleObject* a_Bubble)
	{
		for(size_t i = 0; i < m_GameBubble.size(); i++)
		{
			if(m_GameBubble[i].get() == a_Bubble)
			{
				m_GameBubble.erase(m_GameBubble.begin() + i);
				m_Rendered.erase(m_Rendered.begin() + i);
				break;
			}
		}
	}

	void Clear()
	{
		m_GameBubble.clear();
		m_Rendered.clear();
	}

	std::vector<std::unique_ptr<BubbleObject>>& GetGameBubbles() { return m_GameBubble; }
	std::vector<std::unique_ptr<AnimatedSprite>>& GetRendered() { return m_Rendered; }

private:
	std::vector<std::unique_ptr<AnimatedSprite>> m_Rendered;
	std::vector<std::unique_ptr<BubbleObject>> m_GameBubble;

};
