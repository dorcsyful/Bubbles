#include "Gameplay.h"

#include <iostream>
#include <SFML/Window/Keyboard.hpp>

#include "BubbleMath.h"

void Gameplay::Update(float a_Delta)
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
	{
		Move(Settings::get().GetPointerMoveSpeed() * a_Delta);
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
	{
		Move(Settings::get().GetPointerMoveSpeed() * a_Delta * -1.f);
	}

}

void Gameplay::CheatNextBubble(EBUBBLE_TYPE a_ToDrop)
{
	m_CurrentBubble = a_ToDrop;
}

std::unique_ptr<BubbleObject> Gameplay::CombineBubble(const BubbleObject* a_First, const BubbleObject* a_Second)
{
	sf::Vector2f center = BubbleMath::Lerp(a_First->GetPosition(), a_Second->GetPosition(), 0.5f);
	int i = static_cast<int>(a_First->GetBubbleType()) + 1;
	if (i >= 10) { i = 0; }
	std::unique_ptr<BubbleObject> newBubble = std::make_unique<BubbleObject>(static_cast<EBUBBLE_TYPE>(i));
	newBubble->SetPosition(center);
	m_Score += static_cast<unsigned int>(bubble_weights.at(a_First->GetBubbleType())) * 10;
	
	m_CombineCombo++;
	m_Score += Settings::get().GetComboScore();
	return newBubble;
}

void Gameplay::Move(float a_Direction)
{
	float temp = m_CurrentPosition + a_Direction;

	float x = m_ContainerEdges[0] + Settings::get().BubbleSize(m_CurrentBubble) * Settings::get().GetPixelToMeter();
	if (temp < x) temp = x;

	x = m_ContainerEdges[1] - Settings::get().BubbleSize(m_CurrentBubble) * Settings::get().GetPixelToMeter();
	if (temp > x) temp = x;

	m_CurrentPosition = temp;
}

std::unique_ptr<BubbleObject> Gameplay::Drop(const sf::Vector2f& a_Start)
{
	std::unique_ptr<BubbleObject> newBubble = std::make_unique<BubbleObject>(m_CurrentBubble);
	sf::Vector2f temp = a_Start;
	temp.y -= Settings::get().BubbleSize(m_CurrentBubble) + 0.001f;

	newBubble->SetPosition(temp);
	m_CurrentBubble = m_NextBubble;
	int next = rand() % 10;
	if (next == 8) { next = 10; }
	else if (next == 9) { next = 11; }
	else { next = rand() % 3; }
	m_NextBubble = static_cast<EBUBBLE_TYPE>(next);
	Move(0);
	m_CombineCombo = 0;
	return newBubble;
}

void Gameplay::Reset(float a_WindowWidth)
{
	time_t currentTime = time(nullptr);
	srand(static_cast<unsigned int>(currentTime));
	m_CurrentBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
	m_NextBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
	m_ContainerEdges[0] = a_WindowWidth / 2.f - Settings::get().GetContainerWidth() / 2.f;
	m_ContainerEdges[1] = m_ContainerEdges[0] + Settings::get().GetContainerWidth();
	m_CurrentPosition = 0;
	Move(0);
	m_Score = 0;
}
