#pragma once
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>
#include "BubbleObject.h"

class BubbleWrapper
{
public:
	BubbleWrapper(const std::shared_ptr<BubbleObject>& a_GameObject, const std::shared_ptr<sf::RectangleShape>& a_Rendered) : m_Rendered(a_Rendered), m_GameBubble(a_GameObject) {}

	void Update()
	{
		sf::Vector2f position = m_GameBubble->GetPosition();
		position.y *= -1;
		m_Rendered->setPosition(position);
		m_Rendered->setRotation(0);
	}
	std::shared_ptr<sf::RectangleShape> GetShape() { return m_Rendered; }

private:
	std::shared_ptr<sf::RectangleShape> m_Rendered;
	std::shared_ptr<BubbleObject> m_GameBubble;

};
