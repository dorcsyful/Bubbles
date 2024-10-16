#include "Gameplay.h"

#include <iostream>
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

std::shared_ptr<BubbleObject> Gameplay::CombineBubble(const std::shared_ptr<BubbleObject>& a_First, const std::shared_ptr<BubbleObject>& a_Second)
{
	sf::Vector2f center = BubbleMath::Lerp(a_First->GetPosition(), a_Second->GetPosition(), 0.5f);
	int i = static_cast<int>(a_First->GetBubbleType()) + 1;
	if (i == bubble_sizes.size()) { i = 0; }
	std::shared_ptr<BubbleObject> newBubble = std::make_shared<BubbleObject>(static_cast<EBUBBLE_TYPE>(i));
	newBubble->SetPosition(center);
	m_Score += static_cast<unsigned int>(bubble_weights.at(a_First->GetBubbleType())) * 10;
	return newBubble;
}

void Gameplay::Move(float a_Direction)
{
	float temp = m_CurrentPosition + a_Direction;

	float x = m_ContainerEdges[0] + bubble_sizes.at(m_CurrentBubble) * PIXEL_TO_METER;
	if (temp < x) temp = x;

	x = m_ContainerEdges[1] - bubble_sizes.at(m_CurrentBubble) * PIXEL_TO_METER;
	if (temp > x) temp = x;

	m_CurrentPosition = temp;
}

std::shared_ptr<BubbleObject> Gameplay::Drop(const sf::Vector2f& a_Start)
{
	std::shared_ptr<BubbleObject> newBubble = std::make_shared<BubbleObject>(m_CurrentBubble);
	sf::Vector2f temp = a_Start;
	temp.x /= PIXEL_TO_METER;
	temp.y /= PIXEL_TO_METER;
	temp.y += bubble_sizes.at(m_CurrentBubble);
	temp.y *= -1.f;
	newBubble->SetPosition(temp);
	m_CurrentBubble = m_NextBubble;
	m_NextBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
	Move(0);
	return newBubble;
}
