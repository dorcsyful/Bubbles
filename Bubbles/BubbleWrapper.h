#pragma once
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>

#include "AnimatedSprite.h"
#include "BubbleObject.h"

class BubbleWrapper
{
public:
	BubbleWrapper(const std::shared_ptr<BubbleObject>& a_GameObject, const std::shared_ptr<AnimatedSprite>& a_Rendered) : m_Rendered(a_Rendered), m_GameBubble(a_GameObject) {}

	void Update()
	{
		sf::Vector2f position = m_GameBubble->GetPosition();
		position.x *= PIXEL_TO_METER;
		position.y *= PIXEL_TO_METER;
		position.y *= -1;
		m_Rendered->SetPosition(position);
		m_Rendered->SetRotation(m_GameBubble->GetRotation() * 57.2957795f);
	}
	std::shared_ptr<AnimatedSprite> GetShape() const { return m_Rendered; }
	std::shared_ptr<AnimatedSprite> GetShape() { return m_Rendered; }
	std::shared_ptr<BubbleObject> GetBubble() { return m_GameBubble; }
private:
	std::shared_ptr<AnimatedSprite> m_Rendered;
	std::shared_ptr<BubbleObject> m_GameBubble;

};
