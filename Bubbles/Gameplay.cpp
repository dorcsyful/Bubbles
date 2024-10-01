#include "Gameplay.h"

#include <SFML/Window/Keyboard.hpp>

void Gameplay::Update(float a_Delta)
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
	{
		Move(POINTER_MOVE_SPEED * a_Delta);
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
	{
		Move(POINTER_MOVE_SPEED * a_Delta * -1.f);
	}
}

void Gameplay::CombineBubble(std::shared_ptr<BubbleObject> a_First, std::shared_ptr<BubbleObject> a_Second)
{
}

void Gameplay::Move(float a_Direction)
{
	float temp = m_CurrentPosition + a_Direction;

	float x = m_ContainerEdges[0] + bubble_sizes.at(m_CurrentBubble);
	if (temp < x) temp = x;

	x = m_ContainerEdges[1] - bubble_sizes.at(m_CurrentBubble);
	if (temp > x) temp = x;

	m_CurrentPosition = temp;
}

std::shared_ptr<BubbleObject> Gameplay::Drop(sf::Vector2f a_Start)
{
	std::shared_ptr<BubbleObject> newBubble = std::make_shared<BubbleObject>(m_CurrentBubble);
	newBubble->SetPosition(a_Start);
	m_CurrentBubble = m_NextBubble;
	m_NextBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
	Move(0);
	return newBubble;
}
